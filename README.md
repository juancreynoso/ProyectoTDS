# ProyectoTDS

Este proyecto implementa un mini compilador completo para un lenguaje imperativo simple que soporta los tipos primitivos **integer** y **bool**. El compilador realiza cada una de las etapas clásicas presentes en los compiladores modernos, transformando el código fuente en un archivo de código Assembly x86-64, listo para ser ensamblado y ejecutado.

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

- Tipos de datos soportados: int y bool
- Declaraciones: Variables y Funciones
- Operaciones: Aritméticas (`+`, `-`, `*`, `/`, `%`), lógicas (`&&`, `||`, `!`) y relacionales(`>`, `<`, `==`)
- Analisis sintactico: Verificacion de sintaxis
- Análisis semántico: Verificación de tipos y tabla de símbolos

## Estructura del proyecto

### Directorios principales

- `ast/`
  - Contiene el codigo fuente que genera el arbol de sintaxis abstracto `ast`
- `doc/`
  - Contiene la documentacion del proyecto en formato `html`
- `lexer/`
  - Contiene el analizador sintactico generado con `Flex`
- `sintax/`
  - Contiene el codigo que se encarga de definir la gramatica del lenguaje y generar el parser
- `st/`
  - Contiene el codigo fuente que genera la tabla de simbolos.
- `tac/`
  - Contiene el codigo fuente que genera el codigo de tres direcciones.
- `ass/`
  - Contiene el codigo fuente que genera el codigo assembler.
- `opt/`
  - Contiene el codigo fuente que implementa optimizaciones.
- `test/`: Archivos con código de prueba.

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

Esto genera como salida **únicamente** el archivo Assembly (`.s`) correspondiente en el directorio `output/`.  
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

Primero, si no tiene la imagen, crearla (solo una vez):

```
docker build --platform linux/amd64 -t ctds-build ./docker
```

Luego entrar al entorno de compilación:

```
docker run --platform linux/amd64 -it --rm -v "$PWD":/work -w /work ctds-build bash
```

Dentro del contenedor, compilar:

```
gcc -o program outputs/<archivo>.s
./program
```

## Ejecución automatizada de la suite de tests

Se incluye un runner que automatiza la compilación y ejecución de todos los tests en `test/` usando Docker (útil en macOS) o ejecutándolo localmente en Linux.

Archivos relevantes:

- `scripts/run_tests_docker_v2.sh` — construye la imagen Docker (si falta) y ejecuta la suite dentro del contenedor.
- `scripts/inside_run.sh` — script que se ejecuta dentro del contenedor: compila con `make`, genera `.s` para cada `.ctds`, compila los binarios (enlazando `functions.c`) y ejecuta cada uno con un timeout, mostrando un resumen.

**Uso (macOS / Docker):**

1. Hacer los scripts ejecutables:

```bash
chmod +x scripts/run_tests_docker_v2.sh scripts/inside_run.sh
```

2. Ejecutar el runner (construye la imagen si hace falta):

```bash
./scripts/run_tests_docker_v2.sh
```

Qué hace el runner:

- Reconstruye la imagen Docker (si no existe) usando `docker/Dockerfile`.
- Ejecuta `make` dentro del contenedor para compilar `c-tds`.
- Para cada `test/*.ctds`:
  - Ejecuta `./c-tds test/<file>.ctds` para generar `output/<file>.s`.
  - Compila `output/<file>.s` con `gcc -o output/program_<file> output/<file>.s functions.c`.
  - Ejecuta el binario con `timeout` (por defecto 5s) para evitar colgados.
- Muestra `[OK]`/`[FAIL]` según las aserciones implementadas en `functions.c` y un resumen `passed/failed`.

**Ejecución local en Linux:**

- Si está en Linux x86_64 y tiene las dependencias instaladas (`make`, `gcc`, `bison`, `flex`), puede ejecutar el runner directamente sin Docker:

```bash
make
chmod +x scripts/inside_run.sh
./scripts/inside_run.sh
```

Configuración y opciones:

- Timeout por test: el runner usa 5 segundos por test; si quiere otro valor puedo añadir una bandera `--timeout N`.
- Forzar reconstrucción de la imagen: `docker build --platform linux/amd64 -t ctds-build:latest docker` antes de lanzar el runner.

## Ejecución por etapas

También es posible ejecutar el compilador hasta una etapa específica:

```
./c-tds -target <etapa> test/<archivo>.ctds
```

**Etapas disponibles:**

| Comando | Etapa                                    |
| ------- | ---------------------------------------- |
| lex     | Análisis léxico                          |
| parse   | Análisis sintáctico                      |
| sem     | Análisis semántico                       |
| tac     | Generación de código de tres direcciones |
| ass     | Generación de código assembly            |

En modo normal **solo se genera el archivo `.s`** sin las optimizaciones implementadas.  
Si se desea ver _todos_ los archivos intermedios (`output.lex`, `output.sint`, `output.sem`, `output.ci`), se debe usar:

```
./c-tds -debug test/<archivo>.ctds
```

Esto facilita la inspección y depuración interna del compilador.

Para obtener el archivo `.s` con las optimizaciones se debe ejecutar:

```
./c-tds -opt test/<archivo>.ctds
```

## Optimizaciones implementadas

- **Propagación de constantes** (ej. 10 + 2 → 12)
- **Eliminación de código muerto** (sentencias posteriores a return dentro de un bloque)
- **Optimización de división por 2** (usa sar en vez de idiv)
- **Reutilización de offsets de temporales** para reducir el uso de stack

Estas optimizaciones mejoran la legibilidad y eficiencia del código generado, reduciendo instrucciones y memoria utilizada.

## Limpiar archivos generados

```
make clean
```

## Documentación del proyecto

```
make open-doc
```

## Autor

Juarez Marcelo - Reynoso Juan Cruz
