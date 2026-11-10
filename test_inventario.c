/* Pruebas unitarias simples con assert. Ejecutar: make test */
#include <assert.h>
#include <stdio.h>
#include "../src/inventario.h"

int main(void) {
    Inventario inv; ColaPedidos cola; PilaMovimientos hist;
    inv_init(&inv); cola_init(&cola); pila_init(&hist);

    /* Alta y orden por SKU */
    assert(inv_agregar(&inv, 300, "Tarima", "C-01-1", 10, 5) == 0);
    assert(inv_agregar(&inv, 100, "Caja chica", "A-01-1", 50, 20) == 0);
    assert(inv_agregar(&inv, 200, "Playo", "B-02-3", 8, 10) == 0);
    assert(inv_agregar(&inv, 100, "Duplicado", "A-01-1", 1, 1) == -1);
    assert(inv.total == 3);
    assert(inv.inicio->sku == 100 && inv.inicio->sig->sku == 200);

    /* Búsqueda */
    assert(inv_buscar(&inv, 200) != NULL);
    assert(inv_buscar(&inv, 999) == NULL);

    /* Entrada + deshacer (pila) */
    assert(registrar_entrada(&inv, &hist, 200, 12) == 0);
    assert(inv_buscar(&inv, 200)->existencia == 20);
    assert(deshacer_ultimo(&inv, &hist) == 0);
    assert(inv_buscar(&inv, 200)->existencia == 8);

    /* Pedidos FIFO y backorder */
    cola_encolar(&cola, 1, 100, 30);
    cola_encolar(&cola, 2, 300, 99);   /* no alcanza */
    assert(surtir_siguiente_pedido(&inv, &cola, &hist) == 1);
    assert(inv_buscar(&inv, 100)->existencia == 20);
    assert(surtir_siguiente_pedido(&inv, &cola, &hist) == -3);
    assert(cola.total == 1);           /* regresó a la cola */

    /* Persistencia */
    assert(inv_guardar(&inv, "/tmp/test_inv.csv") == 0);
    Inventario copia; inv_init(&copia);
    assert(inv_cargar(&copia, "/tmp/test_inv.csv") == 3);
    assert(inv_buscar(&copia, 100)->existencia == 20);

    /* Eliminación */
    assert(inv_eliminar(&inv, 200) == 0);
    assert(inv_eliminar(&inv, 200) == -1);
    assert(inv.total == 2);

    inv_liberar(&inv); inv_liberar(&copia); cola_liberar(&cola); pila_liberar(&hist);
    printf("Todas las pruebas pasaron.\n");
    return 0;
}
