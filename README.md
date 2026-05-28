# Algoritmo de Huffman em C

Implementacao completa de um compactador e descompactador usando o algoritmo de Huffman.

## Compilacao

```bash
gcc huffman.c -o huffman
```

No Windows, se estiver usando MinGW:

```powershell
gcc huffman.c -o huffman.exe
```

## Uso

Compactar:

```bash
./huffman -c arquivo.txt compactado.huff
```

Descompactar:

```bash
./huffman -d compactado.huff arquivo_restaurado.txt
```

## Teste rapido

Depois de compilar, rode:

```bash
./huffman -c repeticao.txt repeticao.huff
./huffman -d repeticao.huff repeticao_restaurado.txt
```

No Windows PowerShell, compare os arquivos assim:

```powershell
Compare-Object (Get-Content repeticao.txt) (Get-Content repeticao_restaurado.txt)
```

Se o comando nao mostrar nenhuma diferenca, a descompactacao restaurou o arquivo corretamente.

## Arquivos de teste da entrega

Os arquivos de entrada escolhidos ficam na pasta `testes/`:

| Caso | Arquivo de entrada | Objetivo |
| --- | --- | --- |
| Texto de 1 MB | `testes/entrada_texto_1mb.txt` | Avaliar compactacao em texto longo. |
| Alta repeticao | `testes/entrada_repeticao.txt` | Demonstrar compactacao em dados com muitos simbolos repetidos. |
| Binario ja compactado | `testes/entrada_binaria_compactada.zip` | Demonstrar que arquivos com alta entropia podem ficar maiores. |

Para reproduzir os resultados:

```bash
./huffman -c testes/entrada_repeticao.txt testes/compactados/repeticao.huff
./huffman -d testes/compactados/repeticao.huff testes/restaurados/entrada_repeticao_restaurada.txt

./huffman -c testes/entrada_texto_1mb.txt testes/compactados/texto_1mb.huff
./huffman -d testes/compactados/texto_1mb.huff testes/restaurados/entrada_texto_1mb_restaurada.txt

./huffman -c testes/entrada_binaria_compactada.zip testes/compactados/binario_compactado.huff
./huffman -d testes/compactados/binario_compactado.huff testes/restaurados/entrada_binaria_compactada_restaurada.zip
```

## Formato do arquivo `.huff`

O arquivo compactado usa o formato pedido no enunciado:

1. `Contagem`: 4 bytes, quantidade de bytes unicos.
2. `Tabela`: para cada byte unico, 1 byte do valor original e 4 bytes da frequencia.
3. `Bits de lixo`: 1 byte, valor entre 0 e 7.
4. `Payload`: dados codificados bit a bit.

## Porcentagem de compactacao

Use esta formula:

```text
compactacao (%) = (1 - tamanho_compactado / tamanho_original) * 100
```

Resultados obtidos com os arquivos da pasta `testes/`:

| Caso | Tamanho original | Tamanho compactado | Compactacao |
| --- | ---: | ---: | ---: |
| Texto de 1 MB | 1.048.576 bytes | 545.624 bytes | 47,97% |
| Alta repeticao | 1.024 bytes | 212 bytes | 79,30% |
| Binario ja compactado | 262.370 bytes | 263.655 bytes | -0,49% |

Valores negativos significam que o arquivo compactado ficou maior que o original, o que pode acontecer em arquivos pequenos ou arquivos que ja estao compactados, como PNG e ZIP.
