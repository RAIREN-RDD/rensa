## Rensa project strucure
Let `CWD` be the Current Working Directory.
Let `PROJECT_ROOT` be the project's absolute location.  
Let `RENSA_ROOT` be Rensa install directory absolute location.  

Every template's extension is `.ra`  
For now, assume it's always `.tar.xz`. In the future, `.tar.zstd` and other algos will be supported.


## Create Rensa project
```bash
rensa create <PROJECT_NAME> <TEMPLATE>
```

Let `<TEMPLATE>` be a template name or an URL  
Supported URLs: http, https  

Create directory `<PROJECT_NAME>`  

If `<TEMPLATE>` is not found as a filename in `$RENSA_ROOT/templates`:
 - download `<TEMPLATE>` from the web

Then, extract the resource in memory and replace all occurrences of `__PROJECT_NAME__` into `<PROJECT_NAME>`  
Write all files to `$CWD/<PROJECT_NAME>`  

