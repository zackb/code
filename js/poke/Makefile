VERSION=1.28.3.20

default:
	echo "mer?"

deploy:
	rsync -av . z@z.com:~/web/poke

data:
	curl 'https://pvpoke.com/data/rankings/all/overall/rankings-1500.json?v=$(VERSION)' | jq "map({speciesName,score})" > data/overall-1500.json
	curl 'https://pvpoke.com/data/rankings/all/overall/rankings-2500.json?v=$(VERSION)' | jq "map({speciesName,score})" > data/overall-2500.json
	curl 'https://pvpoke.com/data/rankings/all/overall/rankings-10000.json?v=$(VERSION)' | jq "map({speciesName,score})" > data/overall-10000.json
