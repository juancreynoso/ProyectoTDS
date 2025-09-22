# ProyectoTDS
  
Este proyecto implementa un mini compilador para un lenguaje de programación que soporta dos tipos de datos: `int` y `bool`. El compilador realiza actualmente análisis léxico y sintáctico

## Entregas

- **Primera entrega**  
  Rama: `scanner-parser`

## Características

* Tipos de datos soportados: int y bool
* Declaraciones: Variables y Funciones
* Operaciones: Aritméticas (`+`, `-`, `*`, `/`, `%`), lógicas (`&&`, `||`, `!`) y relacionales(`>`, `<`, `==`)
* Analisis sintactico: Verificacion de sintaxis
* Análisis semántico: Verificación de tipos y tabla de símbolos

##  Estructura del proyecto
### Directorios principales
* `ast`
    *   Contiene el codigo fuente que genera el arbol de sintaxis abstracto `ast`
* `doc`
    *   Contiene la documentacion del proyecto en formato `html`
* `lexer`
    *   Contiene el analizador sintactico generado con `Flex`
*   `sintax/`
    *   Contiene el codigo que se encarga de definir la gramatica del lenguaje y generar el parser
*   `st`
    *   Contiene el codigo fuente que genera la tabla de simbolos.
*   `test`: Archivos con código de prueba.
*   `README.md`: Este archivo.

#### Dependencias
- **gcc** (GNU Compiler Collection) - versión 4.8 o superior
- **make** (GNU Make) - versión 3.81 o superior  
- **flex** (Fast Lexical Analyzer) - versión 2.5 o superior
- **bison** (GNU Parser Generator) - versión 3.0 o superior

## Compilar el proyecto
```
make 
```

## Ejecución de tests
Utilizar archivos del directorio `test/` para probar el funcionamiento del mini compilador.
```
./c-tds -target <etapa> test/<archivo_fuente>
```

* `Etapas disponibles:`
    * lexer → análisis léxico
    * parser → análisis sintáctico

### Ejecutar todas las etapas
Sino se especifica una etapa, el compilador las ejecutara todas en orden
```
./c-tds -target test/<archivo_fuente>
```

## Limpiar archivos generados
```
make clean
```

## Abrir documentacion
```
make open-doc
```

## Autor
Juarez Marcelo - Reynoso Juan Cruz