#!/usr/bin/env python
# Dink: Deployment and Flink Job Management Tool
#
# Wraps the Flink REST API: https://ci.apache.org/projects/flink/flink-docs-release-1.7/monitoring/rest_api.html
# and simplifies operations by following 2 conventions
#
# Requirements: pip install requests boto3
#
# ./dink.py -a run_job -sd s3a://bucket/testing/sp -jn hello-world.jar -jar s3://bucket/testing/hello-world-1.0.0.jar
# ./dink.py -a run_job -sd s3a://bucket/testing/sp -jn fragment-falcon.jar -jar /Users/zack/code/hello-flink/build/libs/hello-flink-1.0.0-SNAPSHOT.jar -p 9
# ./dink.py -a upgrade_job --job_name hello-flink --jar_name hello.jar --savepoint_dir s3://bucket/testing/savepoints --parallelism 2
# ./dink.py -a upgrade_job --job_name hello-flink --jar_name hello.jar --jar_file /Users/zack/code/hello-flink/build/libs/hello-flink-1.0.0-SNAPSHOT.jar --savepoint_dir s3://bucket/testing/savepoints --parallelism 2
#
# To simplify flink deployment some conventions are used:
#   Jar 
#       A jar is uploaded to the jobmanager which uses shared storage. A name is associated with the jar and should be in the form <name>.jar
#       To make things simpler, a jar_name is required when uploading jars
#   Job
#       A job can be stop/started/upagraded using either a new jar (by specifying --jar_file) or using the name of the jar when it was uploaded (<name>.jar) using --jar_name.
#       Multiple jobs can use the same jar but require a unique job_name. The job_name is set in code in StreamExecutionEnvironment.execute(jobName).
#       Example: 
#           jars have been uploaded using the jar_name 'hello.jar'
#           two jobs can be started named hello-world and hello-flink using that same jar. The deployment version of the jar can be managed together or separately 
#
# In most cases you can specify either a jar_name and optionally a jar_file to use with the action. If jar_file is specified dink will upload it to Flink
#
# created by zack on 3/28/19.
#

import sys
import argparse
import requests
import boto3
from time import sleep

# Flink API client
flink = None

MAX_JARS = 3

actions = {}

def main(argv):
    global actions

    actions = {
        'menu': menu,

        'list_jars': list_jars,
        'upload_jar': upload_jar,
        'prune_jars': prune_jars,

        'list_jobs': list_jobs,
        'stop_job': stop_job,
        'run_job': run_job,
        'upgrade_job': upgrade_job
    }


    args = parse_args()
    if args.action not in actions:
        print "Error: I don't know what '%s' is" % args.action
        return 1
    print actions[args.action](args)

    
#####################
#### jar control ####
#####################
def list_jars(args):
    return flink.list_jars_named(args.jar_name)

def upload_jar(args):
    if args.jar_file is None or args.jar_name is None:
        return 'Error: must specify both --jar_name and --jar_file'
    return flink.upload_jar(args.jar_name, get_file_byte_stream(args.jar_file))

def prune_jars(args):
    if args.jar_name is None:
        return 'Error: --jar_name is required'
    return flink.prune_jars(args.jar_name, MAX_JARS)


#####################
#### job control ####
#####################
def list_jobs(args):
    return flink.list_jobs(args.job_id, args.job_name)

def stop_job(args):
    if args.savepoint_dir is None:
        return "Error: must specify --savepoint_dir. I won't stop stop a job without a savepoint"
    return flink.stop_job(get_job(args.job_id, args.job_name)['id'], args.savepoint_dir)

def run_job(args):
    if args.job_name is not None:
        jobs = flink.list_jobs(None, args.job_name)
        for job in jobs:
            if job['state'] == 'RUNNING':
                return "Error: a job with name %s is already RUNNING" % args.job_name

    jar = _upload_or_find_jar(args)

    if jar is None:
        return 'Error: must supply valid --jar_file or a valid --jar_name, use -a list_jars to see existing jars'

    return flink.run_job(jar['id'], args.savepoint, args.program_args, args.parallelism)

def upgrade_job(args):
    jar = _upload_or_find_jar(args)

    if jar is None:
        return 'Error: must supply valid --jar_file or a valid --jar_name, use -a list_jars to see existing jars'

    if args.savepoint_dir is None:
        return "Error: must specify --savepoint_dir"

    job = get_job(args.job_id, args.job_name)

    savepoint = flink.stop_job(job['id'], args.savepoint_dir)
    res = flink.run_job(jar['id'], savepoint, args.program_args, args.parallelism)
    flink.prune_jars(jar['name'], MAX_JARS)
    return res

# upload a jar if supplied in the program arguments, or find the newest already uploaded jar
def _upload_or_find_jar(args):
    if args.jar_file is not None:
        flink.upload_jar(args.jar_name, get_file_byte_stream(args.jar_file))

    return flink.fetch_newest_jar_named(args.jar_name)
    

# get the job in question, either by using the supplied job name and/or id 
# or by prompting the user to choose one
def get_job(job_id = None, job_name = None):
    job = None
    jobs = flink.list_jobs(job_id, job_name)
    running = []
    for j in jobs:
        if j['state'] == 'RUNNING':
            running.append(j)

    if len(running) > 1:
        raise Exception("Multiple jobs with the same name running: %s" % running)
    elif len(running) == 1:
        job = running[0]

    return select_job() if job is None else job

# get a pointer to a file byte stream, either local file:// or s3://
def get_file_byte_stream(filename):
    if filename.startswith('s3'):
        idx = [pos for pos, char in enumerate(filename) if char == '/']
        bucket = filename[idx[1] + 1:idx[2]]
        key = filename[idx[2] + 1:]
        return fetch_s3_file_byte_stream(bucket, key)
    else:
        return open(filename)

# return a pointer to the body of an object in s3
def fetch_s3_file_byte_stream(bucket, key): 
    print "Getting S3 file pointer: %s %s" % (bucket, key)
    client = boto3.client('s3')
    d = client.get_object(Bucket=bucket, Key=key)
    return d['Body']

def menu(args):
    return 'Error: please specify an action with -a'
    
# prompt the user to choose a job to act on
def select_job():
    jobs = flink.list_jobs()
    print 'state\t\t\tname\t\t\tid\t\t'
    for job in jobs:
        print "%s\t\t\t%s\t\t%s\t\t" % (job['state'], job['name'], job['id'])

    job = None

    while job == None:
        sys.stdout.write('Job ID: ')
        sys.stdout.flush()
        ans = sys.stdin.readline().strip()
        for j in jobs:
            if j['id'] == ans:
                job = j
                break
        # job = next(j for j in jobs if j['id'] == ans) 
    return job
        
def parse_args():
    global flink

    parser = argparse.ArgumentParser(description='Dink: Deployment for Flink, duh.')
    parser.add_argument('-H', '--host' , 
                        nargs='?', required=True,
                        help='hostname or ip address of the flink cluster (will follow redirects to the current jobmanager)')

    parser.add_argument('-P', '--port' , 
                        nargs='?', const=8081, type=int, default=8081,
                        help='REST API port (default 8081)')

    parser.add_argument('-a', '--action' , 
                        nargs='?', const='menu', default='menu', choices=actions.keys(),
                        help='dink action to perform')

    parser.add_argument('-j', '--job_id', 
                        help='ID of the running flink job to act on')

    parser.add_argument('-jn', '--job_name', 
                        help='name of the running flink job to act on')

    parser.add_argument('-jar', '--jar_file',
                        help='jar file to upload, ex: s3://bucket/jars/hello-flink.jar')

    parser.add_argument('-jrn', '--jar_name', 
                        help='name of the jar file to upload/overwrite, must end with .jar because flink is silly')

    parser.add_argument('-pa', '--program_args', 
                        help='(optional) program arguments to pass to the job')

    parser.add_argument('-p', '--parallelism', type=int,
                        help='(optional) job parallelism, defaults to cluster default')

    parser.add_argument('-sd', '--savepoint_dir', help='location of savepoints, ex: s3://bucket/testing/savepoints')
    parser.add_argument('-s', '--savepoint', help='location of the savepoint to restore from')

    args = parser.parse_args()

    flink = FlinkClient(args.host, args.port)

    if args.jar_file is not None:
        if args.jar_name is None:
            raise Exception('Must specify both --job_name and --jar_file when uploading a new jar')

    return args

class FlinkClient:

    # flink jobmanager host, will follow redirects
    host = None
    # Flink REST API port
    port = -1

    def __init__(self, host, port):
        """Initialize a flink REST API client using the given host/ip and port"""
        self.host = host
        self.port = port

    def list_jars(self):
        """List all jars available to the flink jobmanager.

        Return: array of dicts descriping the available jars
            id (str):       the Jar ID
            name (str):     name of the jar
            date (int64):   unix epoch milliseconds of the jar upload time
        """
        jars = []
        rs = self.fetch_json('jars')
        for r in rs['files']:
            jars.append({'id': r['id'], 'name': r['name'], 'date': r['uploaded']})
        return jars

    def list_jars_named(self, jar_name):
        """List jars that have the gien name.

        Return: array of dicts descriping the jars with the supplied name
            id (str):       the Jar ID
            name (str):     name of the jar
            date (int64):   unix epoch milliseconds of the jar upload time
        """
        res = []
        jars = self.list_jars()
        for jar in jars:
            if jar_name is None or jar['name'] == jar_name:
                res.append(jar)
        return res

    def fetch_newest_jar_named(self, jar_name):
        """Find the newest jar by upload time of the given jar name.

        Return: dict descriping the newest jar of the given name
            id (str):       the Jar ID
            name (str):     name of the jar
            date (int64):   unix epoch milliseconds of the jar upload time
        """
        jars = self.list_jars_named(jar_name)
        if len(jars) == 0:
            return None

        srt = sorted(jars, key = lambda j : j['date'], reverse = True)
        return srt[0]

    # upload a jar with a name and body
    def upload_jar(self, jar_name, body):
        """Upload a jar to flink. Contents of the jar are a file byte stream frome local filesystem or S3.

        Keyword arguments:
        jar_name (str):   required. name of the jar to identify with flink
        body (file):      required. stream of bytes representing the contents of the jar

        Return: dict descriping the jar upload
            status(str):        'success' if the uplaod was successful
            filename (str):     the path on the flink job server where the jar is being stored
        """
        print "Uploading jar: %s" % jar_name
        files = {'jarfile': (jar_name, body, 'application/x-java-archive')}
        url = self.make_url('jars/upload')
        r = requests.post(url, files=files).json()
        print "Upload successful: %s" % r['filename']
        return r

    def delete_jar(self, jar_id):
        """Delete a jar from flink jar storage by id

        Keyword arguments:
        jar_id (str):   required. the flink ID of the jar

        Return: dict descriping the deletion
            id (str):           the jar ID
            name (str):         the jar name specified when it was uploaded with --jar_name
            date (int64):       unix epoch milliseconds of the jar upload time
        """
        print "Deleting: %s" % jar_id
        return requests.delete(self.make_url("jars/%s" % jar_id))

    def prune_jars(self, jar_name, maxx):
        """Prune historical jar versions by name and maximum number to keep. Checks for jars matching a name and deletes older
        jars, keeping up to maxx number.

        Keyword arguments:
        jar_name (str):   required. name of the flink jar to prune

        Return: array of dicts descriping the deletion
            id (str):           the jar ID
            name (str):         the jar name specified when it was uploaded with --jar_name
            date (int64):       unix epoch milliseconds of the jar upload time
        """
        jars = self.list_jars_named(jar_name)
        srt = sorted(jars, key = lambda j : j['date'], reverse = True)
        delete = srt[maxx:]
        for d in delete:
            print "Deleting: %s %d" % (d['name'], d['date'])
            self.delete_jar(d['id'])
        return delete


    def list_job_ids(self):
        """Fetch all registered Job IDs from flink. 

        Return: array of strings where each entry is a Job ID
        """
        job_ids  = []
        res = self.fetch_json('jobs')
        for r in res['jobs']:
            job_ids.append(r['id'])
        return job_ids 

    def list_jobs(self, job_id = None, job_name = None):
        """Find a jobs by id and/or name

        Keyword arguments:
        job_id (str):   optional. the ID of the job to return
        job_name (str): optional. the name of the job(s) to return

        Return: array of dicts descriping the job
            id (str):          the job ID
            name (str):         the job name specified in the `execute(string)` java method
            status (str):       the state of the job, one of RUNNING, CANCELLED, FINISHED, CREATED, FAILED, SCHEDULED
            start-time (int64): unix epoch milliseconds of the job submission time, or -1 if not applicable
        """
        res = []
        job_ids = [job_id] if job_id is not None else self.list_job_ids()
        jobs = [self.fetch_job_details(job_id) for job_id in job_ids]
        for job in jobs:
            if job_name is None or job['name'] == job_name:
                res.append(job)
        return res

    def fetch_job_details(self, job_id):
        """Get details about a flink Job given the Job ID

        Keyword arguments:
        job_id (str):   required. the ID of the job to return details for

        Return: dict descriping the job
            id (str):          the job ID
            name (str):         the job name specified in the `execute(string)` java method
            status (str):       the state of the job, one of RUNNING, CANCELLED, FINISHED, CREATED, FAILED, SCHEDULED
            start-time (int64): unix epoch milliseconds of the job submission time, or -1 if not applicable
        """
        json = self.fetch_json("jobs/%s" % job_id)
        job = {}
        job['id'] = json['jid']
        job['name'] = json['name']
        job['state'] = json['state']
        return job

    # run a job continuing from a savepoint
    def run_job(self, jar_id, savepoint, program_args = None, parallelism = None):
        """Submit a job to flink and start it.

        Keyword arguments:
        jar_id (str):   required. the ID of the jar code to use for the job
        savepoint (str):   optional. location of savepoint to resume the job from
        program_args (str):   optional. arguments to pass the job in the form: --var val
        parallelism (int):   optional. job parallelism, the number of tasks used for the job, defaults to cluster default

        Return: dict descriping the job start result
            jobid (str):        the job ID
        """
        data = {
            'savepointPath': savepoint,
            'programArgs': program_args,
            'allowNonRestoredState': 'true',
            'parallelism': parallelism
        }

        print "Running job: %s with savepoint: %s" % (jar_id, savepoint)
        r = requests.post(self.make_url("jars/%s/run" % jar_id), json=data).json()
        return r

    def check_savepoint_status(self, job_id, trigger_id):
        """After a savepoint has been requested, this routine polls the savepoint's progress and returns when
            the savepoint has completed.

        Keyword arguments:
        job_id (str):   required. the Job ID which is being savepointed
        trigger_id (str):   required. savepoint trigger_id as returned by stop_job (jobs/<id>/savepoints)

        Return: string with the filesystem (or S3) location of the savepoint
        """
        status = None
        json = None
        while status != 'COMPLETED':
            sleep(1)
            json = self.fetch_json("jobs/%s/savepoints/%s" % (job_id, trigger_id))
            status = json['status']['id']
        return json['operation']['location']

    # cancels a job and triggers a savepoint. returns the savepoint
    def stop_job(self, job_id, savepoint_dir):
        """Cancles a job and triggers a savepoint.

        Keyword arguments:
        job_id (str):   required. the Job ID which is being savepointed
        savepoint_dir (str):   required. where to store the savepoint

        Return: string with the filesystem (or S3) location of the savepoint taken when the job was stopped
        """
        data = {'target-directory': savepoint_dir, 'cancel-job': 'true'}
        r = requests.post(self.make_url("jobs/%s/savepoints" % job_id), json=data).json()
        trigger_id = r['request-id']
        savepoint = flink.check_savepoint_status(job_id, trigger_id)
        print "Saved savepoint to: %s" % savepoint
        return savepoint

    def fetch_json(self, endpoint):
        """Make a request to the flink REST API and convert the results to JSON. If an error occurs
            either via HTTP or in the body of the flink response, an Exception is raised

        Keyword arguments:
        endpoint (str):   required. the HTTP endpoint of the API to hit

        Return: json response from flink REST API
        """
        json = requests.get(self.make_url(endpoint)).json()
        if 'errors' in json:
            raise Exception("Failed API call: %s" % json['errors'])
        return json

    def make_url(self, endpoint):
        return "http://%s:%d/%s" % (self.host, self.port, endpoint)

if __name__ == "__main__":
    main(sys.argv[1:])
