# ProyectoTDS
  
Este proyecto implementa un mini compilador para un lenguaje de programación que soporta dos tipos de datos: `int` y `bool`. El compilador realiza actualmente análisis léxico y sintáctico

## Entregas

- **Primera entrega**  
    Rama: `scanner-parser`

- **Segunda entrega**  
    Rama: `semantic`

- **Tercer entrega**  
    Rama: `cod-inter`

- **Cuarta entrega**  
    Rama: `object-code`

- **Quinta entrega**
    Rama: `optimization`

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
*  `sintax/`
    *   Contiene el codigo que se encarga de definir la gramatica del lenguaje y generar el parser
*  `st`
    *   Contiene el codigo fuente que genera la tabla de simbolos.
* `tac`
    *  Contiene el codigo fuente que genera el codigo de tres direcciones.
* `ass`
    *  Contiene el codigo fuente que genera el codigo assembler.
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

Los archivos de prueba se encuentran en el directorio `test/`.

Para compilar un archivo `.ctds` simplemente ejecutar:
```
./c-tds test/<archivo>.ctds
```

Esto genera como salida **únicamente** el archivo Assembly (`.s`) correspondiente en el directorio `outputs/`.  
Ese archivo `.s` es el equivalente a nuestro “ejecutable” generado por el compilador.

### Compilación del `.s` a un binario real

#### Linux

Si está en Linux (x86_64 / amd64), simplemente:
```
gcc -o program outputs/<archivo>.s
./program
```


#### macOS

En macOS **no se puede compilar directamente el `.s` generado**, ya que el compilador produce assembly para arquitectura **x86_64**, mientras que macOS utiliza ARM64.

Para compilar y ejecutar correctamente, se debe usar Docker con una imagen Linux `amd64`.

Primero, si no tiene la imagen, crearla (solo una vez): `docker build --platform linux/amd64 -t my-ubuntu-gcc ./docker`

Luego entrar al entorno de compilación:
```
--platform linux/amd64 -it --rm -v "$PWD":/work -w /work my-ubuntu-gcc bash
```

Finalmente, compilar:
```
gcc -o program outputs/<archivo>.s
./program
```

---

## Ejecución por etapas

También es posible ejecutar el compilador hasta una etapa específica:
```
./c-tds -target <etapa> test/<archivo>.ctds
```

**Etapas disponibles:**

| Comando   | Etapa |
|--------|-------------|
| lex    | Análisis léxico |
| parse  | Análisis sintáctico |
| sem    | Análisis semántico |
| tac    | Generación de código de tres direcciones |
| ass    | Generación de código assembly |

En modo normal **solo se genera el archivo `.s`**.  
Si se desea ver *todos* los archivos intermedios (`output.lex`, `output.sint`, `output.sem`, `output.ci`), se debe usar:
```
./c-tds -debug test/<archivo>.ctds
```

Esto facilita la inspección y depuración interna del compilador.

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