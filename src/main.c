#include <stdio.h>
#include <string.h>
#include "inventario.h"

#define ARCHIVO "datos/inventario.csv"

static int leer_int(const char *msg) {
    int v;
    printf("%s", msg);
    while (scanf("%d", &v) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Entrada invalida. %s", msg);
    }
    return v;
}

static void leer_texto(const char *msg, char *buf, int n) {
    printf("%s", msg);
    int c; while ((c = getchar()) != '\n' && c != EOF) {}   /* limpia buffer */
    if (fgets(buf, n, stdin)) buf[strcspn(buf, "\n")] = '\0';
}

static void menu(void) {
    printf("\n===== SISTEMA DE INVENTARIO DE ALMACEN =====\n");
    printf(" 1. Listar inventario (por SKU)\n");
    printf(" 2. Listar por existencia (quicksort)\n");
    printf(" 3. Buscar producto\n");
    printf(" 4. Alta de producto\n");
    printf(" 5. Baja de producto\n");
    printf(" 6. Registrar entrada (recibo)\n");
    printf(" 7. Capturar pedido\n");
    printf(" 8. Ver pedidos pendientes\n");
    printf(" 9. Surtir siguiente pedido\n");
    printf("10. Deshacer ultimo movimiento\n");
    printf("11. Reporte de reorden\n");
    printf("12. Guardar\n");
    printf(" 0. Guardar y salir\n");
}

int main(void) {
    Inventario inv;  ColaPedidos cola;  PilaMovimientos hist;
    inv_init(&inv);  cola_init(&cola);  pila_init(&hist);

    int n = inv_cargar(&inv, ARCHIVO);
    printf(n >= 0 ? "Se cargaron %d productos de " ARCHIVO "\n" : "No se encontro " ARCHIVO ", se inicia vacio.\n", n);

    int folio = 1000, op;
    do {
        menu();
        op = leer_int("Opcion: ");
        switch (op) {
        case 1: inv_listar(&inv); break;
        case 2: inv_listar_por_existencia(&inv); break;
        case 3: {
            Producto *p = inv_buscar(&inv, leer_int("SKU: "));
            if (p) printf("%s | Ubicacion %s | Existencia %d | Minimo %d\n", p->nombre, p->ubicacion, p->existencia, p->minimo);
            else printf("No existe ese SKU.\n");
            break;
        }
        case 4: {
            char nombre[MAX_NOMBRE], ubic[MAX_UBICACION];
            int sku = leer_int("SKU: ");
            leer_texto("Nombre: ", nombre, sizeof nombre);
            printf("Ubicacion (ej. A-03-2): ");
            if (scanf("%11s", ubic) != 1) break;
            int ex = leer_int("Existencia inicial: ");
            int mn = leer_int("Punto de reorden: ");
            printf(inv_agregar(&inv, sku, nombre, ubic, ex, mn) == 0 ? "Producto agregado.\n" : "Ese SKU ya existe.\n");
            break;
        }
        case 5: printf(inv_eliminar(&inv, leer_int("SKU a eliminar: ")) == 0 ? "Eliminado.\n" : "No existe.\n"); break;
        case 6: {
            int sku = leer_int("SKU: "), cant = leer_int("Cantidad recibida: ");
            printf(registrar_entrada(&inv, &hist, sku, cant) == 0 ? "Entrada registrada.\n" : "SKU o cantidad invalidos.\n");
            break;
        }
        case 7: {
            int sku = leer_int("SKU: "), cant = leer_int("Cantidad: ");
            if (!inv_buscar(&inv, sku) || cant <= 0) { printf("SKU o cantidad invalidos.\n"); break; }
            cola_encolar(&cola, ++folio, sku, cant);
            printf("Pedido %d en cola.\n", folio);
            break;
        }
        case 8: cola_listar(&cola); break;
        case 9: {
            int r = surtir_siguiente_pedido(&inv, &cola, &hist);
            if (r > 0) printf("Pedido %d surtido.\n", r);
            else if (r == -1) printf("No hay pedidos.\n");
            else if (r == -3) printf("Existencia insuficiente: el pedido pasa al final de la cola (backorder).\n");
            else printf("SKU inexistente; pedido descartado.\n");
            break;
        }
        case 10: printf(deshacer_ultimo(&inv, &hist) == 0 ? "Movimiento revertido.\n" : "No hay movimientos.\n"); break;
        case 11: inv_reporte_reorden(&inv); break;
        case 12: case 0:
            printf(inv_guardar(&inv, ARCHIVO) == 0 ? "Guardado en " ARCHIVO "\n" : "Error al guardar.\n");
            break;
        default: printf("Opcion no valida.\n");
        }
    } while (op != 0);

    inv_liberar(&inv);  cola_liberar(&cola);  pila_liberar(&hist);
    return 0;
}
