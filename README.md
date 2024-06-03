## Running

### Pulling and running
```bash
docker pull ghcr.io/bytedjinn/silo:latest
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest 
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest bob
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest bob e bob is a painter
docker run -v <path>:/usr/src/app/data ghcr.io/bytedjinn/silo:latest bob t today bob went to the shops
```
where `<path>` is the absolute path to a "data" folder you created. 

It is recommended to create an alias of the prefix. 


### Building and running
```bash
docker run --rm -i -v ${pwd}/data:/usr/src/app/data $(docker build -q .) foo eternal bar

```

## Alias

### PowerShell
Run `notepad $PROFILE`. 

Add the text:
```
function silo {
    param (
        [string[]]$Args
    )
    docker run -v <path> ghcr.io/bytedjinn/silo:latest @Args
}
```
Run `. $PROFILE`. 