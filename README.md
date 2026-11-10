# Sistema de Inventario de Almacén en C

Aplicación de consola para controlar el inventario de un almacén: altas y bajas de productos, recibo de mercancía, captura y surtido de pedidos, reporte de reorden e historial con "deshacer".

Lo hice a partir de mi experiencia como **auxiliar de operaciones de almacén**: cada función corresponde a un proceso real (recibo, surtido, conteo cíclico, ubicaciones pasillo-rack-nivel).

## Estructuras de datos utilizadas

| Estructura | Uso en el sistema | Por qué |
|---|---|---|
| **Lista doblemente ligada** (ordenada por SKU) | Catálogo de productos | Inserción/eliminación en O(1) una vez ubicado el nodo; búsqueda con corte temprano por estar ordenada |
| **Cola (FIFO)** | Pedidos pendientes | El primer pedido capturado es el primero en surtirse; si no hay existencia pasa al final (*backorder*) |
| **Pila (LIFO)** | Historial de movimientos | Permite deshacer el último recibo o surtido |
| **Quicksort** | Reporte por existencia | Ordena un arreglo de punteros sin modificar la lista original |

## Funcionalidades

- Alta, baja y búsqueda de productos por SKU
- Registro de entradas (recibo de mercancía)
- Captura de pedidos y surtido en orden de llegada
- Manejo de *backorder* cuando no hay existencia suficiente
- Deshacer el último movimiento
- Reporte de productos por debajo del punto de reorden
- Persistencia en archivo CSV
- Pruebas unitarias con `assert`
- Gestión de memoria dinámica sin fugas (`malloc`/`free`)

## Cómo ejecutarlo

Requisitos: `gcc` y `make` (Linux, macOS o WSL en Windows).

```bash
make          # compila
./inventario  # ejecuta
make test     # corre las pruebas
```

## Ejemplo

```
===== SISTEMA DE INVENTARIO DE ALMACEN =====
11. Reporte de reorden
Opcion: 11
== Productos en o por debajo del punto de reorden ==
SKU      Producto                     Ubicacion    Exist.   Minimo
--------------------------------------------------------------------
1002     Playo stretch 18in           A-01-2           15       20  <- reordenar
1005     Etiqueta termica 4x6         B-03-2            8       25  <- reordenar
```

## Estructura del proyecto

```
inventario-almacen-c/
├── src/
│   ├── inventario.h   # estructuras y prototipos
│   ├── inventario.c   # lógica: lista, cola, pila, quicksort, CSV
│   └── main.c         # menú de consola
├── tests/
│   └── test_inventario.c
├── datos/
│   └── inventario.csv
└── Makefile
```

## Próximas mejoras

- [ ] Búsqueda por nombre
- [ ] Árbol binario de búsqueda para búsquedas en O(log n)
- [ ] Reporte de conteo cíclico (diferencias entre sistema y físico)

---
Autor: **Saúl Maximiliano García Ramírez**, estudiante de Ingeniería en Sistemas Computacionales (ESCOM-IPN)
