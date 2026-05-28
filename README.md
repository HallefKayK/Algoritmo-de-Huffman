# Algoritmo de Huffman em C

Implementacao completa de um compactador e descompactador de arquivos usando o algoritmo de Huffman.

O programa funciona por linha de comando e trabalha byte a byte, entao pode ser usado com arquivos de texto e arquivos binarios.

## Funcionalidades

- Compactacao de arquivos para o formato `.huff`.
- Descompactacao de arquivos `.huff`.
- Reconstrucao do arquivo original sem perda de bytes.
- Uso de arvore de Huffman e fila de prioridade com min-heap.
- Gravacao de cabecalho com tabela de frequencias para permitir a descompactacao.

## Compilacao

No Windows com MinGW:

```powershell
gcc huffman.c -o huffman.exe
```

Em Linux ou macOS:

```bash
gcc huffman.c -o huffman
```

## Como usar

Compactar:

```powershell
.\huffman.exe -c arquivo_original arquivo_compactado.huff
```

Descompactar:

```powershell
.\huffman.exe -d arquivo_compactado.huff arquivo_restaurado
```

Exemplo simples:

```powershell
.\huffman.exe -c repeticao.txt repeticao.huff
.\huffman.exe -d repeticao.huff repeticao_restaurado.txt
```

## Estrutura do arquivo `.huff`

O arquivo compactado possui:

| Campo | Tamanho | Descricao |
| --- | ---: | --- |
| Contagem | 4 bytes | Quantidade de bytes unicos presentes no arquivo original. |
| Tabela | Variavel | Para cada byte unico, grava 1 byte do valor original e 4 bytes da frequencia. |
| Bits de lixo | 1 byte | Quantidade de bits extras no ultimo byte do payload. |
| Payload | Variavel | Conteudo original codificado bit a bit com Huffman. |

O cabecalho e necessario porque o descompactador precisa reconstruir a mesma arvore de Huffman usada na compactacao.

## Arquivos de teste

Os principais arquivos de teste ficam na pasta `testes/`.

| Caso | Entrada | Compactado | Restaurado |
| --- | --- | --- | --- |
| Texto de 1 MB | `testes/entrada_texto_1mb.txt` | `testes/compactados/texto_1mb.huff` | `testes/restaurados/entrada_texto_1mb_restaurada.txt` |
| Alta repeticao | `testes/entrada_repeticao.txt` | `testes/compactados/repeticao.huff` | `testes/restaurados/entrada_repeticao_restaurada.txt` |
| Binario ja compactado | `testes/entrada_binaria_compactada.zip` | `testes/compactados/binario_compactado.huff` | `testes/restaurados/entrada_binaria_compactada_restaurada.zip` |

Tambem ha um teste extra na raiz do projeto:

| Caso | Entrada | Compactado |
| --- | --- | --- |
| Imagem JPG | `imagem-teste.jpg` | `imagem-teste.huff` |

## Comandos para reproduzir os testes

Alta repeticao:

```powershell
.\huffman.exe -c testes\entrada_repeticao.txt testes\compactados\repeticao.huff
.\huffman.exe -d testes\compactados\repeticao.huff testes\restaurados\entrada_repeticao_restaurada.txt
```

Texto de 1 MB:

```powershell
.\huffman.exe -c testes\entrada_texto_1mb.txt testes\compactados\texto_1mb.huff
.\huffman.exe -d testes\compactados\texto_1mb.huff testes\restaurados\entrada_texto_1mb_restaurada.txt
```

Binario ja compactado:

```powershell
.\huffman.exe -c testes\entrada_binaria_compactada.zip testes\compactados\binario_compactado.huff
.\huffman.exe -d testes\compactados\binario_compactado.huff testes\restaurados\entrada_binaria_compactada_restaurada.zip
```

Imagem JPG:

```powershell
.\huffman.exe -c imagem-teste.jpg imagem-teste.huff
```

## Verificacao dos arquivos restaurados

Para provar que o arquivo restaurado e igual ao original, compare os hashes.

Exemplo:

```powershell
Get-FileHash testes\entrada_repeticao.txt
Get-FileHash testes\restaurados\entrada_repeticao_restaurada.txt
```

Se os hashes forem iguais, os arquivos sao identicos byte a byte.

## Resultados de compactacao

Formula usada:

```text
compactacao (%) = (1 - tamanho_compactado / tamanho_original) * 100
```

Resultados obtidos:

| Caso | Tamanho original | Tamanho compactado | Compactacao |
| --- | ---: | ---: | ---: |
| Alta repeticao | 1.024 bytes | 212 bytes | 79,30% |
| Texto de 1 MB | 1.048.576 bytes | 545.624 bytes | 47,97% |
| Binario ja compactado ZIP | 262.370 bytes | 263.655 bytes | -0,49% |
| Imagem JPG | 10.636 bytes | 11.894 bytes | -11,83% |

Valores negativos significam que o arquivo compactado ficou maior que o original. Isso e esperado em arquivos pequenos ou ja compactados, como ZIP, JPG, PNG, MP3 e outros formatos com alta entropia, porque o algoritmo ainda precisa gravar o cabecalho.

## Resumo do funcionamento

Na compactacao, o programa le o arquivo original byte a byte, conta a frequencia dos 256 bytes possiveis, cria uma min-heap, constroi a arvore de Huffman juntando sempre os dois menores nos, gera os codigos binarios de cada byte e grava o arquivo `.huff`.

Na descompactacao, o programa le o cabecalho, reconstroi a arvore usando a tabela de frequencias e percorre o payload bit a bit para recuperar cada byte original.
