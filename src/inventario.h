#ifndef INVENTARIO_H
#define INVENTARIO_H

#define MAX_NOMBRE 50
#define MAX_UBICACION 12

/* ---------- Producto: nodo de lista doblemente ligada ---------- */
typedef struct Producto {
    int sku;
    char nombre[MAX_NOMBRE];
    char ubicacion[MAX_UBICACION];   /* p. ej. "A-03-2" (pasillo-rack-nivel) */
    int existencia;
    int minimo;                      /* punto de reorden */
    struct Producto *sig;
    struct Producto *ant;
} Producto;

typedef struct {
    Producto *inicio;
    int total;
} Inventario;

/* ---------- Pedido: nodo de cola (FIFO) ---------- */
typedef struct Pedido {
    int folio;
    int sku;
    int cantidad;
    struct Pedido *sig;
} Pedido;

typedef struct {
    Pedido *frente;
    Pedido *final;
    int total;
} ColaPedidos;

/* ---------- Movimiento: nodo de pila (historial para deshacer) ---------- */
typedef enum { MOV_ENTRADA, MOV_SALIDA } TipoMov;

typedef struct Movimiento {
    TipoMov tipo;
    int sku;
    int cantidad;
    struct Movimiento *sig;
} Movimiento;

typedef struct {
    Movimiento *tope;
    int total;
} PilaMovimientos;

/* Inventario (lista ligada ordenada por SKU) */
void inv_init(Inventario *inv);
int  inv_agregar(Inventario *inv, int sku, const char *nombre, const char *ubic, int exist, int minimo);
Producto *inv_buscar(Inventario *inv, int sku);
int  inv_eliminar(Inventario *inv, int sku);
void inv_listar(const Inventario *inv);
void inv_reporte_reorden(const Inventario *inv);
void inv_liberar(Inventario *inv);

/* Ordenamiento: copia la lista a un arreglo y aplica quicksort por existencia */
void inv_listar_por_existencia(const Inventario *inv);

/* Cola de pedidos */
void cola_init(ColaPedidos *c);
void cola_encolar(ColaPedidos *c, int folio, int sku, int cantidad);
int  cola_desencolar(ColaPedidos *c, Pedido *out);
void cola_listar(const ColaPedidos *c);
void cola_liberar(ColaPedidos *c);

/* Pila de movimientos */
void pila_init(PilaMovimientos *p);
void pila_push(PilaMovimientos *p, TipoMov tipo, int sku, int cantidad);
int  pila_pop(PilaMovimientos *p, Movimiento *out);
void pila_liberar(PilaMovimientos *p);

/* Operaciones de almacén */
int  registrar_entrada(Inventario *inv, PilaMovimientos *hist, int sku, int cantidad);
int  surtir_siguiente_pedido(Inventario *inv, ColaPedidos *cola, PilaMovimientos *hist);
int  deshacer_ultimo(Inventario *inv, PilaMovimientos *hist);

/* Persistencia en archivo CSV */
int  inv_guardar(const Inventario *inv, const char *ruta);
int  inv_cargar(Inventario *inv, const char *ruta);

#endif
