

Don't write `int, char` but `int8_t, int16_t, int32_t`.

put doxygen comment before each function

``` 

/**
*
*/
void my_function(void) {

}
```

int the begginning of file 

```
/**
*
* @file my_file.c
* @author David.Goodenough
*
*/  
```


include put macro to avoid multiple def
```
#ifndef HEADER_H
#define HEADER_H
....
#endif
```

Macro with upper character

```
#define MY_MACRO
```

https://www.doxygen.nl/manual/docblocks.html

https://cyber.gouv.fr/publications/regles-de-programmation-pour-le-developpement-securise-de-logiciels-en-langage-c