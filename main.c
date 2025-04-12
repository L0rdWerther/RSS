#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// Função para escrever a informação de um elemento XML no arquivo HTML
void escrever_informacao(xmlNode *node, FILE *arquivo_html, const char *elemento, const char *formato) {
    for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next) {
        // Verifica se o nó atual é o elemento desejado
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((const char *)cur_node->name, elemento) == 0) {
            char *conteudo = (char *)xmlNodeGetContent(cur_node);
            if (conteudo) {
                // Escreve o conteúdo do elemento no arquivo HTML
                fprintf(arquivo_html, formato, conteudo, conteudo);
                xmlFree(conteudo);
            }
        }
    }
}

// Função para processar nós XML (canal e itens)
void processar_nos(xmlNode *node, FILE *arquivo_html) {
    // Escreve as informações do canal
    escrever_informacao(node, arquivo_html, "title", "<h1>Título do canal: %s</h1>\n");
    escrever_informacao(node, arquivo_html, "description", "<p>Descrição do canal: %s</p>\n");
    escrever_informacao(node, arquivo_html, "link", "<p>Link do canal: <a href=\"%s\">%s</a></p>\n");

    // Processa cada item dentro do canal
    for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((const char *)cur_node->name, "item") == 0) {
            // Escreve as informações de cada item (notícia)
            escrever_informacao(cur_node->children, arquivo_html, "title", "<h2>Título da notícia: %s</h2>\n");
            escrever_informacao(cur_node->children, arquivo_html, "description", "<p>Descrição da notícia: %s</p>\n");
            escrever_informacao(cur_node->children, arquivo_html, "link", "<p>Link: <a href=\"%s\">%s</a></p>\n");
            escrever_informacao(cur_node->children, arquivo_html, "pubDate", "<p>Data de publicação: %s</p>\n");
        }
    }
}

// Função para analisar o feed RSS
void analisar_feed_rss(const char *nome_arquivo) {
    // Carrega o arquivo XML
    xmlDoc *doc = xmlReadFile(nome_arquivo, NULL, 0);
    xmlNode *root_element = xmlDocGetRootElement(doc);

    // Abre o arquivo HTML para escrita
    FILE *arquivo_html = fopen("../output.html", "w");
    fprintf(arquivo_html, "<html>\n<body>\n");

    // Percorre os nós do XML e processa o canal e itens
    for (xmlNode *cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((const char *)cur_node->name, "channel") == 0) {
            processar_nos(cur_node->children, arquivo_html);
        }
    }

    // Finaliza o arquivo HTML
    fprintf(arquivo_html, "</body>\n</html>");
    fclose(arquivo_html);

    // Libera a memória usada pelo documento XML
    xmlFreeDoc(doc);
    xmlCleanupParser();
}

// Função principal
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Para rodar o programa, indique um arquivo rss/xml: %s <rss_feed.xml>\n", argv[0]);
        return 1;
    }

    // Analisa o arquivo RSS fornecido como argumento
    analisar_feed_rss(argv[1]);
    return 0;
}
