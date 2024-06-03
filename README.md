## Running

```bash
docker run --rm -i -v ${pwd}/data:/usr/src/app/data $(docker build -q .) foo eternal bar
docker build -t silo .

```

## Usage

```
silo                        outputs a list of people created
silo <person>               outputs eternal and transient information about the person
silo <person> eternal       allows you to edit the eternal information
silo <person> transient     adds a new transient entry
```