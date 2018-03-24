#include "fila.h"
 
void push(heap_t *h, struct mensagem data) 
{
    if (h->len + 1 >= h->size)
    {
        h->size = h->size ? h->size * 2 : 4;
        h->message = (struct mensagem *)realloc(h->message, h->size * sizeof (struct mensagem));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->message[j].prioridade < data.prioridade)
    {
        h->message[i] = h->message[j];
        i = j;
        j = j / 2;
    }
    h->message[i].prioridade = data.prioridade;
    h->message[i].eh_bastao = data.eh_bastao;
    //strcpy(h->message[i].eh_bastao,data.eh_bastao);
    strcpy(h->message[i].mensagem,data.mensagem);
    strcpy(h->message[i].origem,data.origem);
    strcpy(h->message[i].destino,data.destino);
    h->len++;
}
 
struct mensagem *pop(heap_t *h)
{
    int i, j, k;
    if (!h->len) {
        return NULL;
    }
    struct mensagem *data = malloc(sizeof(struct mensagem)); // = h->message[1];
    data->prioridade = h->message[1].prioridade;
    data->eh_bastao = h->message[1].eh_bastao;
    //strcpy(data->eh_bastao,h->message[1].eh_bastao);
    strcpy(data->mensagem,h->message[1].mensagem);
    strcpy(data->origem,h->message[1].origem);
    strcpy(data->destino,h->message[1].destino);
    h->message[1] = h->message[h->len];
    h->len--;
    i = 1;
    while (1) {
        k = i;
        j = 2 * i;
        if (j <= h->len && h->message[j].prioridade > h->message[k].prioridade) {
            k = j;
        }
        if (j + 1 <= h->len && h->message[j + 1].prioridade > h->message[k].prioridade) {
            k = j + 1;
        }
        if (k == i) {
            break;
        }
        h->message[i] = h->message[k];
        i = k;
    }
    h->message[i] = h->message[h->len + 1];
   
    return data;
}