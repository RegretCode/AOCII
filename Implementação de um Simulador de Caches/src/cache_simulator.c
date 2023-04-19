#include <stdio.h>
#include <stdlib.h>
#include <math.h> // Biblioteca utilizada para o cálculo de potênciação e logarítmo
#include <time.h> // Biblioteca utilizada para a geração de números aleatórios
// #include <byteswap.h> // Biblioteca utilizada para o "swap" de bytes de um endereço. Mingw não compila essa biblioteca.

/*
====================================================================================================================================================================================
Nome: Marcelo Petitot Rezende e Ricardo Coutinho Cordeiro

Implementação do Simulador

Deverá ser implementado um simulador funcional de caches em uma linguagem de programação (livre
escolha), este simulador deverá ser parametrizável (configurações da cache), quanto ao número de
conjuntos, tamanho do bloco, nível de associatividade e política de substituição. Considere que a cache é
endereçada à bytes e o endereço possui 32 bits.

A configuração de cache deverá ser repassada por linha de comando e formatada com os seguintes
parâmetros (o arquivo de entrada poderá ter extensão):

cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada

Onde cada um destes campos possui o seguinte significado:
• cache_simulator - nome do arquivo de execução principal do simulador (todos devem usar este nome, independente da linguagem escolhida);
• nsets - número de conjuntos na cache (número total de “linhas” ou “entradas” da cache);
• bsize - tamanho do bloco em bytes;
• assoc - grau de associatividade (número de vias ou blocos que cada conjunto possui);
• substituição - política de substituição, que pode ser Random (R), FIFO (F) ou L (LRU);
• flag_saida - flag que ativa o modo padrão de saída de dados;
• arquivo_de_entrada - arquivo com os endereços para acesso à cache.

O tamanho da cache é dado pelo produto do número de conjuntos na cache (<nsets>), tamanho do bloco em bytes (<bsize>) e associatividade (<assoc>).

Para caches associativas, a política de substituição default será randômica. A implementação da opção de escolha por outros modos será considerada como bônus.

Formato de saída:

A saída do simulador será um relatório de estatísticas com o número total de acessos, número total hits e misses (os misses deverão ser classificados em compulsórios,
capacidade e conflito), que poderá ser exibido em dois formatos, de acordo com o valor do campo flag_saida:
• flag_saida = 0
	o Formato livre, pode conter textos com labels para facilitar a visualização. Ex: Taxa de hits = 90%.
• flag_saida = 1
	o Formato padrão de saída que deverá respeitar a seguinte ordem: Total de acessos, Taxa de hit, Taxa de miss, Taxa de miss compulsório, Taxa de miss de capacidade,
	Taxa de miss de conflito
	o Ex: 100000, 0.95, 0.06, 0.01, 0.02, 0.03

Arquivo de entrada:

O arquivo de entrada será utilizado como entrada para o simulador (armazenado em formato binário) que conterá os endereços requisitados à cache
(cada endereço possui 32 bits, em formato big endian). Quatro arquivos de teste serão fornecidos para auxiliar na verificação dos códigos, contendo 100, 1000,
10.000 e 186676 endereços. Cada um deles estará disponível no formato binário exigido, e também em .txt para facilitar a visualização dos valores e o entendimento
do comportamento da cache.

Exemplo:
Considerando a seguinte linha de comando, utilizando o arquivo de entrada “bin_100.bin”:

				cache_simulator 256 4 1 R 1 bin_100.bin

O resultado esperado para a saída é: 100 0.92 0.08 1.00 0.00 0.00

====================================================================================================================================================================================
*/

// Declaração da função
int byte_Swap(int address_Value);

int main(int argc, char *argv[])
{
	// Tratamento de exceção caso o usuário rode o programa com o número incorreto de parâmetros
	if (argc != 7)
	{
		printf("Erro! Para executar o programa, passe os argumentos: ");
		printf("./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada\n");
		exit(EXIT_FAILURE);
	}

	// Permite a geração de números aleatórios
	srand(time(NULL));

	// Variáveis: Contadores
	int access = 0, hit = 0, miss = 0, comp_Miss = 0, cap_Miss = 0, conf_Miss = 0;

	// Variáveis: Percentagens
	float hit_Ratio = 0, miss_Ratio = 0, comp_Miss_Ratio = 0, cap_Miss_Ratio = 0, conf_Miss_Ratio = 0;

	// Variáveis: Endereços do arquivo binário
	int address_Value, address_Index, address_Tag;

	// Variáveis: Parâmetros
	int nsets = atoi(argv[1]);		// Número de conjuntos
	int bsize = atoi(argv[2]);		// Tamanho do bloco
	int assoc = atoi(argv[3]);		// Associatividade
	char *substituicao = argv[4];	// Política de substituição
	int flagOut = atoi(argv[5]);	// Flag de saída
	char *arquivoEntrada = argv[6]; // Arquivo binário de entrada

	// Variáveis: Auxiliar
	int aux, added, full;

	// Vetores que representarão a cache
	int cache_Valid[nsets][assoc]; // Bit de validade
	int cache_Tag[nsets][assoc];   // Tag

	// Inicializa os vetores zerados
	for (int i = 0; i < nsets; i++)
	{
		for (int j = 0; j < assoc; j++)
		{
			cache_Valid[i][j] = 0;
			cache_Tag[i][j] = 0;
		}
	}

	// Cálculos para encontrar os bits do endereço
	int offset_Num = log2(bsize);				 // Deslocamento
	int index_Num = log2(nsets);				 // Índice
	int tag_Num = 32 - (offset_Num + index_Num); // Tag

	// Abre o arquivo binário
	FILE *bin_File;
	bin_File = fopen(arquivoEntrada, "rb");

	// Loop para leitura do arquivo binário
	while (fread(&address_Value, sizeof(int), 1, bin_File) != 0)
	{
		access++;
		added = 0; // Esta variável impede que um endereço seja adicionado em mais de uma posição na cache

		// Caso esteja utilizando a biblioteca "byteswap.h", descomentar esta linha de código e comentar a próxima
		// address_Value = __bswap_32 ( address_Value );

		// Caso esteja rodando o programa em uma máquina windows / não consiga compilar a biblioteca "byteswap.h"
		address_Value = byte_Swap(address_Value);

		// Aplica a máscara aos bits para obter os valores desejados
		address_Tag = address_Value >> (offset_Num + index_Num);
		address_Index = (address_Value >> offset_Num) & ((int)pow(2, index_Num) - 1);
		// address_Index = address_Value % nsets; // Não parece funcionar: Os resultados não batem com os templates

		// Verifica nos vetores da cache se houve um miss compulsório ou um hit
		for (int k = 0; k < assoc; k++)
		{
			if (cache_Valid[address_Index][k] == 0 && added != 1)
			{
				miss++;
				comp_Miss++;
				cache_Valid[address_Index][k] = 1;
				cache_Tag[address_Index][k] = address_Tag;
				added = 1;
			}
			else
			{
				if (cache_Tag[address_Index][k] == address_Tag && added != 1)
				{
					hit++;
					added = 1;
				}
			}
		}

		// Se não houve nem miss compulsório e nem hit, então só resta miss de capacidade ou de conflito
		if (added == 0)
		{
			// Verifica se há espaços livres na cache
			for (int i = 0; i < nsets; i++)
			{
				for (int j = 0; j < assoc; j++)
				{
					if (cache_Valid[i][j] == 0)
					{
						full = 0;
					}
					else
					{
						full = 1;
					}
				}
			}

			// Se a cache estiver cheia = Miss de capacidade. Resolve o miss utilizando política de substituição "random"
			if (full == 1 && *substituicao == 'R')
			{
				miss++;
				cap_Miss++;
				aux = (rand() % (assoc));
				cache_Tag[address_Index][aux] = address_Tag;

				// Se tiver espaços vazios = Miss de conflito. Resolve o miss utilizando política de substituição "random"
			}
			else if (full == 0 && *substituicao == 'R')
			{
				miss++;
				conf_Miss++;
				aux = (rand() % (assoc));
				cache_Tag[address_Index][aux] = address_Tag;
			}
		}
	}

	// Cálculo das taxas
	hit_Ratio = (float)hit / access;
	miss_Ratio = (float)miss / access;
	comp_Miss_Ratio = (float)comp_Miss / miss;
	cap_Miss_Ratio = (float)cap_Miss / miss;
	conf_Miss_Ratio = (float)conf_Miss / miss;

	// Modelo de resultado de saida
	if (flagOut == 1)
	{
		printf("%d %.4f %.4f %.2f %.2f %.2f", access, hit_Ratio, miss_Ratio, comp_Miss_Ratio, cap_Miss_Ratio, conf_Miss_Ratio);
	}
	else
	{
		printf("==================================================\n\n");
		printf("Status\n\n");
		printf("Tamanho total da cache: %d Bytes\n\n", nsets * bsize * assoc);
		printf("Quantidade de bits para o offset: %d Bits\n", offset_Num);
		printf("Quantidade de bits para o index: %d Bits\n", index_Num);
		printf("Quantidade de bits para o tag: %d Bits\n\n", tag_Num);
		printf("Total de acessos: %d\n\n", access);
		printf("Taxa de acertos: %.2f%%\n", hit_Ratio * 100);
		printf("Taxa de erros: %.2f%%\n", miss_Ratio * 100);
		printf("Taxa de erros compulsorios: %.2f%%\n", comp_Miss_Ratio * 100);
		printf("Taxa de erros de capcidade: %.2f%%\n", cap_Miss_Ratio * 100);
		printf("Taxa de erros de conflito: %.2f%%\n\n", conf_Miss_Ratio * 100);
		printf("==================================================\n");
	}

	return 0;
}

// Inverte os bytes de um endereço de 32 bits;
int byte_Swap(int address_Value)
{
	int address_Inverse;

	address_Inverse = ((address_Value >> 24) & 0xFF) |
					  ((address_Value << 8) & 0xFF0000) |
					  ((address_Value >> 8) & 0xFF00) |
					  ((address_Value << 24) & 0xFF000000);

	return address_Inverse;
}
