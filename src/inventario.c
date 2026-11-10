#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventario.h"

/* =================== INVENTARIO (lista doblemente ligada) =================== */

void inv_init(Inventario *inv) {
    inv->inicio = NULL;
    inv->total = 0;
}

/* Inserta manteniendo la lista ordenada por SKU. Regresa 0 si ok, -1 si el SKU ya existe. */
int inv_agregar(Inventario *inv, int sku, const char *nombre, const char *ubic, int exist, int minimo) {
    if (inv_buscar(inv, sku) != NULL) return -1;

    Producto *nuevo = malloc(sizeof(Producto));
    if (!nuevo) return -2;
    nuevo->sku = sku;
    strncpy(nuevo->nombre, nombre, MAX_NOMBRE - 1);
    nuevo->nombre[MAX_NOMBRE - 1] = '\0';
    strncpy(nuevo->ubicacion, ubic, MAX_UBICACION - 1);
    nuevo->ubicacion[MAX_UBICACION - 1] = '\0';
    nuevo->existencia = exist;
    nuevo->minimo = minimo;
    nuevo->sig = nuevo->ant = NULL;

    if (inv->inicio == NULL || sku < inv->inicio->sku) {
        nuevo->sig = inv->inicio;
        if (inv->inicio) inv->inicio->ant = nuevo;
        inv->inicio = nuevo;
    } else {
        Producto *act = inv->inicio;
        while (act->sig && act->sig->sku < sku) act = act->sig;
        nuevo->sig = act->sig;
        nuevo->ant = act;
        if (act->sig) act->sig->ant = nuevo;
        act->sig = nuevo;
    }
    inv->total++;
    return 0;
}

Producto *inv_buscar(Inventario *inv, int sku) {
    Producto *act = inv->inicio;
    /* Como la lista está ordenada, podemos parar antes */
    while (act && act->sku <= sku) {
        if (act->sku == sku) return act;
        act = act->sig;
    }
    return NULL;
}

int inv_eliminar(Inventario *inv, int sku) {
    Producto *p = inv_buscar(inv, sku);
    if (!p) return -1;
    if (p->ant) p->ant->sig = p->sig; else inv->inicio = p->sig;
    if (p->sig) p->sig->ant = p->ant;
    free(p);
    inv->total--;
    return 0;
}

static void imprimir_encabezado(void) {
    printf("%-8s %-28s %-10s %8s %8s\n", "SKU", "Producto", "Ubicacion", "Exist.", "Minimo");
    printf("--------------------------------------------------------------------\n");
}

static void imprimir_producto(const Producto *p) {
    printf("%-8d %-28s %-10s %8d %8d%s\n", p->sku, p->nombre, p->ubicacion,
           p->existencia, p->minimo, p->existencia <= p->minimo ? "  <- reordenar" : "");
}

void inv_listar(const Inventario *inv) {
    if (!inv->inicio) { printf("Inventario vacio.\n"); return; }
    imprimir_encabezado();
    for (Producto *p = inv->inicio; p; p = p->sig) imprimir_producto(p);
    printf("Total de SKUs: %d\n", inv->total);
}

void inv_reporte_reorden(const Inventario *inv) {
    int n = 0;
    printf("== Productos en o por debajo del punto de reorden ==\n");
    for (Producto *p = inv->inicio; p; p = p->sig) {
        if (p->existencia <= p->minimo) {
            if (n == 0) imprimir_encabezado();
            imprimir_producto(p);
            n++;
        }
    }
    if (n == 0) printf("Todo en niveles correctos.\n");
}

void inv_liberar(Inventario *inv) {
    Producto *p = inv->inicio;
    while (p) { Producto *s = p->sig; free(p); p = s; }
    inv_init(inv);
}

/* ---- Quicksort sobre arreglo de punteros (de menor a mayor existencia) ---- */
static void intercambiar(Producto **a, Producto **b) { Producto *t = *a; *a = *b; *b = t; }

static int particion(Producto **arr, int bajo, int alto) {
    int pivote = arr[alto]->existencia;
    int i = bajo - 1;
    for (int j = bajo; j < alto; j++) {
        if (arr[j]->existencia < pivote) intercambiar(&arr[++i], &arr[j]);
    }
    intercambiar(&arr[i + 1], &arr[alto]);
    return i + 1;
}

static void quicksort(Producto **arr, int bajo, int alto) {
    if (bajo < alto) {
        int pi = particion(arr, bajo, alto);
        quicksort(arr, bajo, pi - 1);
        quicksort(arr, pi + 1, alto);
    }
}

void inv_listar_por_existencia(const Inventario *inv) {
    if (inv->total == 0) { printf("Inventario vacio.\n"); return; }
    Producto **arr = malloc(sizeof(Producto *) * inv->total);
    if (!arr) return;
    int i = 0;
    for (Producto *p = inv->inicio; p; p = p->sig) arr[i++] = p;
    quicksort(arr, 0, inv->total - 1);
    imprimir_encabezado();
    for (i = 0; i < inv->total; i++) imprimir_producto(arr[i]);
    free(arr);
}

/* =================== COLA DE PEDIDOS (FIFO) =================== */

void cola_init(ColaPedidos *c) { c->frente = c->final = NULL; c->total = 0; }

void cola_encolar(ColaPedidos *c, int folio, int sku, int cantidad) {
    Pedido *n = malloc(sizeof(Pedido));
    if (!n) return;
    n->folio = folio; n->sku = sku; n->cantidad = cantidad; n->sig = NULL;
    if (c->final) c->final->sig = n; else c->frente = n;
    c->final = n;
    c->total++;
}

int cola_desencolar(ColaPedidos *c, Pedido *out) {
    if (!c->frente) return -1;
    Pedido *n = c->frente;
    *out = *n;
    c->frente = n->sig;
    if (!c->frente) c->final = NULL;
    free(n);
    c->total--;
    return 0;
}

void cola_listar(const ColaPedidos *c) {
    if (!c->frente) { printf("No hay pedidos pendientes.\n"); return; }
    printf("%-8s %-8s %-8s\n", "Folio", "SKU", "Cant.");
    for (Pedido *p = c->frente; p; p = p->sig)
        printf("%-8d %-8d %-8d\n", p->folio, p->sku, p->cantidad);
    printf("Pedidos en cola: %d\n", c->total);
}

void cola_liberar(ColaPedidos *c) {
    Pedido tmp;
    while (cola_desencolar(c, &tmp) == 0) {}
}

/* =================== PILA DE MOVIMIENTOS (LIFO) =================== */

void pila_init(PilaMovimientos *p) { p->tope = NULL; p->total = 0; }

void pila_push(PilaMovimientos *p, TipoMov tipo, int sku, int cantidad) {
    Movimiento *m = malloc(sizeof(Movimiento));
    if (!m) return;
    m->tipo = tipo; m->sku = sku; m->cantidad = cantidad;
    m->sig = p->tope;
    p->tope = m;
    p->total++;
}

int pila_pop(PilaMovimientos *p, Movimiento *out) {
    if (!p->tope) return -1;
    Movimiento *m = p->tope;
    *out = *m;
    p->tope = m->sig;
    free(m);
    p->total--;
    return 0;
}

void pila_liberar(PilaMovimientos *p) {
    Movimiento tmp;
    while (pila_pop(p, &tmp) == 0) {}
}

/* =================== OPERACIONES DE ALMACEN =================== */

int registrar_entrada(Inventario *inv, PilaMovimientos *hist, int sku, int cantidad) {
    Producto *p = inv_buscar(inv, sku);
    if (!p || cantidad <= 0) return -1;
    p->existencia += cantidad;
    pila_push(hist, MOV_ENTRADA, sku, cantidad);
    return 0;
}

/* Surte el pedido más antiguo. Si no hay existencia suficiente, lo regresa al final de la cola. */
int surtir_siguiente_pedido(Inventario *inv, ColaPedidos *cola, PilaMovimientos *hist) {
    Pedido ped;
    if (cola_desencolar(cola, &ped) != 0) return -1;          /* cola vacía */
    Producto *p = inv_buscar(inv, ped.sku);
    if (!p) return -2;                                         /* SKU inexistente: se descarta */
    if (p->existencia < ped.cantidad) {
        cola_encolar(cola, ped.folio, ped.sku, ped.cantidad);  /* backorder */
        return -3;
    }
    p->existencia -= ped.cantidad;
    pila_push(hist, MOV_SALIDA, ped.sku, ped.cantidad);
    return ped.folio;
}

int deshacer_ultimo(Inventario *inv, PilaMovimientos *hist) {
    Movimiento m;
    if (pila_pop(hist, &m) != 0) return -1;
    Producto *p = inv_buscar(inv, m.sku);
    if (!p) return -2;
    if (m.tipo == MOV_ENTRADA) p->existencia -= m.cantidad;
    else p->existencia += m.cantidad;
    return 0;
}

/* =================== PERSISTENCIA CSV =================== */

int inv_guardar(const Inventario *inv, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) return -1;
    fprintf(f, "sku,nombre,ubicacion,existencia,minimo\n");
    for (Producto *p = inv->inicio; p; p = p->sig)
        fprintf(f, "%d,%s,%s,%d,%d\n", p->sku, p->nombre, p->ubicacion, p->existencia, p->minimo);
    fclose(f);
    return 0;
}

int inv_cargar(Inventario *inv, const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) return -1;
    char linea[256];
    int cargados = 0;
    if (!fgets(linea, sizeof linea, f)) { fclose(f); return 0; }  /* salta encabezado */
    while (fgets(linea, sizeof linea, f)) {
        int sku, exist, minimo;
        char nombre[MAX_NOMBRE], ubic[MAX_UBICACION];
        if (sscanf(linea, "%d,%49[^,],%11[^,],%d,%d", &sku, nombre, ubic, &exist, &minimo) == 5) {
            if (inv_agregar(inv, sku, nombre, ubic, exist, minimo) == 0) cargados++;
        }
    }
    fclose(f);
    return cargados;
}
