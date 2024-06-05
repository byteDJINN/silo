## Running - cmake

```
cd build
cmake ..
cmake --build .
```

## Running - Docker

### Pulling and running
```
docker pull ghcr.io/bytedjinn/silo:latest
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest 
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest bob
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest bob e bob is a painter
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest bob t today bob went for a run
```
where `<path>` is the absolute path to a "data" folder you created. 

It is recommended to create an alias of the prefix. 


### Building and running
```
docker run --rm -i -v ${pwd}/data:/usr/src/app/data $(docker build -q .) foo eternal bar
docker build -t ghcr.io/bytedjinn/silo:latest . && docker run --rm -v ${pwd}/data:/usr/src/app/data ghcr.io/bytedjinn/silo:latest foo eternal bar

```

## Alias

### PowerShell
Run `notepad $PROFILE`. 

Add the text:
```
function silo {

    $baseCommand = 'docker run -v "<path>:/usr/src/app/data" ghcr.io/bytedjinn/silo:latest'
    $fullCommand = $baseCommand + " " + ($Args -join " ")
    
    Invoke-Expression $fullCommand
}

```
Run `. $PROFILE`. 