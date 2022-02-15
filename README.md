# SimuDraco

Protótipo de simulador feito para a equipe de competição DRACO VOLANS


# Build

## Passo 1...
Primeiramente, é necessário baixar o repositório clicando no botão verde na parte de cima dessa página e em baixar repositório

![Capture](https://user-images.githubusercontent.com/82895172/153978963-fc1f74c2-fb0b-4ca3-9e42-bb9f63e738f9.PNG)

Para fazer a build é necessário ter baixado no mínimo [cmake 3.18](https://cmake.org/download/), [algum compilador para c/c++ em sua máquina](https://visualstudio.microsoft.com/pt-br/downloads/) e [python installado em seu computador](https://www.python.org/downloads/)

Após ter baixado os programas acima, basta abrir o prompt de comando em seu computador e escrever o seguinte


```
cd caminho_para_o_diretório_que_deseja_utilizar
cmake --B build
```


Agora só aguardar que o cmake vai fazer todos os arquivos necessários para compilar no diretório build
## Passo 2...

Depois que o processo anterior terminar, você terá diversos arquivos no diretório build

![Capture](https://user-images.githubusercontent.com/82895172/153979290-e167f38e-1e85-4395-a997-72372282ed27.PNG)

Agora só encontrar os arquivos específicos para sua máquina e rodá-los

No visual studio, por exemplo, basta abrir o arquivo .sln e compilar o projeto SimuDraco

# Exemplos

Agora que você compilou o programa, basta abrir o arquivo .exe que se encontra no diretório build

![Capture](https://user-images.githubusercontent.com/82895172/153980692-a223b932-85f6-4967-a1db-1f6b1342fc09.PNG)

Você agora deverá ver uma janela como essa aberta...

![Screenshot (3)](https://user-images.githubusercontent.com/82895172/153980901-554909df-7277-474c-a606-e596fe9de61b.png)

Para testar o que você pode fazer com esse programa, basta criar um arquivo python em algum local que você deseje

e colar esse código!

```
import SimuDracoPython as draco
import math


class Teste:
    
    def __init__(self):
        
        self.grafico = draco.Graph()
        self.grafico.name = "Meu primeiro gráfico!"
        self.grafico.graph_type = draco.Lines
        self.grafico.graph_function = self.minha_funcao_update
        
        
        
    def minha_funcao_update(self,deltaTime,currentTime):
        
        
        return {"x":currentTime,"y":math.cos(2*currentTime)}    
        
```

e clicar em Menu > Add Python File no programa...

![Screenshot (4)](https://user-images.githubusercontent.com/82895172/153981601-c6c8d611-0c5c-47ec-9c2a-322731b9e058.png)

e clicar no arquivo que você criou

![Capture](https://user-images.githubusercontent.com/82895172/153981674-66e03547-db1a-4667-b89d-73d4c1fc481f.PNG)

agora basta clicar em play e você pode ver o gráfico de uma função cosseno!

![Capture](https://user-images.githubusercontent.com/82895172/153981809-c137acaf-04e5-42af-9044-8e73e016f18f.PNG)




