# 🎬 Servidor de Streaming RTP (H.264) em C

Este projeto é um **Servidor RTP (Real-Time Transport Protocol)** leve e de alto desempenho escrito em **C** para sistemas Linux/macOS. Ele lê um fluxo de vídeo bruto **H.264** (`.h264`) de um arquivo local, fatiando-o dinamicamente em unidades NAL (NALUs), organizando-as em pacotes RTP válidos e transmitindo-as via UDP para um reprodutor de vídeo (*client*) na rede local.

---

## 📌 O que o código faz?

O servidor simula uma transmissão de streaming ao vivo de 25 FPS através do seguinte fluxo:

1. **Leitura Íntegra**: Carrega o arquivo de vídeo H.264 completo para a memória (`readFile`), garantindo que nenhum frame de vídeo seja cortado de forma arbitrária nas bordas de blocos.
2. **Parsing de H.264 (NALUs)**: Varre o arquivo localizando os *start codes* do H.264 (`0x000001` ou `0x00000001`) usando funções em `AVC.c` para segmentar o fluxo em unidades de dados independentes (NALUs).
3. **Empacotamento RTP**: Monta os cabeçalhos RTP correspondentes (Versão, Seq, Timestamp, SSRC) em `RTPEnc.c` e escolhe o melhor tipo de encapsulamento de acordo com o tamanho do pacote:
   * **Single NAL Unit**: Para pacotes menores ou iguais a 1400 bytes.
   * **Aggregation Packets (STAP-A)**: Junta múltiplas NALUs pequenas em um único pacote RTP.
   * **Fragmentation Units (FU-A)**: Divide NALUs gigantes (como I-Frames) maiores de 1400 bytes em pequenos fragmentos de rede para respeitar a MTU.
4. **Envio via Socket UDP**: Transmite os pacotes estruturados sobre UDP para o endereço e porta configurados (por padrão, `127.0.0.1:1234`), utilizando QoS/DSCP para priorização de tráfego de mídia em rede local.

---

## 📂 Estrutura do Projeto

* 📄 **`main.c`**: Ponto de entrada do programa. Configura os parâmetros de rede, carrega o arquivo de vídeo e inicia o fluxo de transmissão.
* 📄 **`RTPEnc.c` & `RTPEnc.h`**: Lógica central de montagem de pacotes RTP (criação de cabeçalhos, fracionamento FU-A e agregação STAP-A).
* 📄 **`AVC.c` & `AVC.h`**: Analisador de fluxo de vídeo (H.264), responsável por localizar os bytes de sincronia (*start codes*).
* 📄 **`Network.c` & `Network.h`**: Camada de rede. Inicializa os sockets UDP e lida com o envio real de pacotes com suporte a QoS.
* 📄 **`Utils.c` & `Utils.h`**: Utilitários auxiliares de leitura de arquivos e serialização de dados de 16/32 bits.
* 📄 **`play.sdp`**: Arquivo de Descrição de Sessão (SDP), utilizado por players externos para configurar e escutar o fluxo de vídeo.
* 📄 **`Sample.h264`**: Vídeo bruto de exemplo utilizado para streaming.

---

## 🛠️ Pré-requisitos

Antes de começar, garanta que você tem instalado no seu sistema:

* **Compilador C** (como `gcc` ou `clang`) e utilitário `make` para compilação.
* **FFmpeg / ffplay** (ou player VLC) para reproduzir o streaming em tempo real.
  * No Ubuntu/Debian: `sudo apt install build-essential ffmpeg vlc`

---

## 🚀 Como Rodar o Código (Passo a Passo)

Siga os três passos simples abaixo para compilar e iniciar o streaming local:

### Passo 1: Compilar o Servidor
Abra o seu terminal no diretório do servidor e compile utilizando o `Makefile`:

```bash
cd RTP-Server
make clean && make
```
Isso criará o arquivo executável chamado `RTPServer`.

---

### Passo 2: Iniciar o Player de Vídeo
O player precisa estar aberto e escutando antes do servidor enviar os dados, ou logo após iniciá-lo. Escolha **uma** das duas opções abaixo:

#### Opção A: Usando o `ffplay` (Via Terminal - Altamente recomendado)
Abra um **novo terminal** na pasta raiz do projeto (onde está o arquivo `play.sdp`) e execute:

```bash
cd /home/luis/RTP
ffplay -protocol_whitelist "file,rtp,udp" play.sdp
```
> **Nota**: O parâmetro `-protocol_whitelist` é obrigatório para autorizar o ffplay a usar conexões locais de rede definidas no arquivo `.sdp`.

#### Opção B: Usando o VLC Media Player
1. Abra o **VLC**.
2. Vá em **Mídia > Abrir Arquivo...** (ou simplesmente arraste o arquivo).
3. Selecione o arquivo `play.sdp` localizado na raiz do projeto `/home/luis/RTP/play.sdp`.

---

### Passo 3: Iniciar a Transmissão do Servidor
No terminal onde você compilou o servidor (dentro da pasta `RTP-Server`), execute o binário:

```bash
./RTPServer
```

Você verá a saída do console listando o tamanho de cada pacote NAL e RTP enviado. O player (`ffplay` ou `VLC`) começará a renderizar e reproduzir o vídeo de amostra instantaneamente!

---

## 🧹 Limpeza

Para remover os arquivos compilados temporários (`.o`) e o binário executável gerado:

```bash
cd RTP-Server
make clean
```