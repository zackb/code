import Foundation
import Swifter
// import HttpSwift

let cond = NSCondition()

func main() {
    
    print("starting")
    setup()
    
    let server = HttpServer()
    server["/hello/:msg"] = { request in
        let accepts = request.headers["accepts"] ?? "text/html"
        let message = "Hello, \(request.params[":msg"] ?? "World")"
        
        var body:HttpResponseBody = .text(message)
        
        if accepts.contains("json") {
            body = .json(["message": message])
        } else if accepts.contains("html") {
            body = .htmlBody(message)
        }
        
        
        return .ok(body)
    }
    
    do {
        try server.start()
    } catch {
        print("Error: \(error)")
    }
    
    /*
    let server = Server()
    
    server.get("/hello/{id}") { request in
        print(request.queryParams["state"])
        return .ok(request.routeParams["id"]!)
    }

    do {
        try server.run()
    } catch {
        print("error")
    }
     */

    cond.wait()
    print("exiting")
}

func setup() {
    let signals = [SIGINT, SIGTERM, SIGKILL]
    for sig in signals {
        signal(sig, signalHandler)
    }
}

func signalHandler(signal: Int32) {
    print("caught ", signal)
    cond.signal()
}

main()
