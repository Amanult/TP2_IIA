#include "xlsxwriter.h"
#include <stdio.h>

int main() {

    //exemplo para perceber como funciona o xlsxwriter.h

    // 1. Criar o objeto workbook e o ficheiro de saída
    lxw_workbook  *workbook  = workbook_new("relatorio_c.xlsx");
    if (!workbook) {
        printf("Erro ao criar o workbook!\n");
        return 1;
    }

    // 2. Adicionar uma folha de trabalho
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // 3. Criar um objeto format para texto em negrito
    lxw_format *bold_format = workbook_add_format(workbook);
    format_set_bold(bold_format);

    // 4. Escrever dados formatados e não formatados
    // (Linha, Coluna, String, Formato)
    worksheet_write_string(worksheet, 0, 0, "Nome", bold_format);
    worksheet_write_string(worksheet, 0, 1, "Idade", bold_format);


    worksheet_write_string(worksheet, 2, 0, "Bob", NULL);
    worksheet_write_number(worksheet, 2, 1, 25, NULL);

    // 5. Fechar o workbook (salva o ficheiro)
    return workbook_close(workbook);
}