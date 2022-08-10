.PHONY: data deploy
VERSION=1.29.4.4

default:
	echo "mer?"

deploy:
	rsync -av . zackbart@zackbartel.com:~/web/poke

data:
	curl 'https://pvpoke.com/data/rankings/all/overall/rankings-1500.json?v=$(VERSION)' | jq "map({speciesName,score})" > data/overall-1500.json
	curl 'https://pvpoke.com/data/rankings/all/overall/rankings-2500.json?v=$(VERSION)' | jq "map({speciesName,score})" > data/overall-2500.json
	curl 'https://pvpoke.com/data/rankings/all/overall/rankings-10000.json?v=$(VERSION)' | jq "map({speciesName,score})" > data/overall-10000.json
	curl 'https://pogoapi.net/api/v1/pokemon_evolutions.json' | jq "map({speciesName: .pokemon_name, evolutions: [.evolutions[].pokemon_name]})" > data/pokemon_evolutions.json
