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

Exemplo:

```text
Original: 1000 bytes
Compactado: 650 bytes
Compactacao: (1 - 650 / 1000) * 100 = 35%
```

Valores negativos significam que o arquivo compactado ficou maior que o original, o que pode acontecer em arquivos pequenos ou arquivos que ja estao compactados, como PNG e ZIP.
