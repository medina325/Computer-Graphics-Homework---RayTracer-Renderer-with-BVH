Trabalho - P2 - Computação Gráfica

Autores:
Gabriel Medina Braga (medina_cdz@hotmail.com)
Mário de Araújo Carvalho (mariodearaujocarvalho@gmail.com)

Descrição: 
Todas as atividades foram realizadas. 

A1: O método drawCamera(c) foi implementado na classe SceneEditor. Os valores de F (near) e B (far) são obtidos da câmera, porém, como o intuito é ilustrar o volume de vista, os valores de F e B são multiplicados, respectivamente, pelos valores 54.00 e 0.003. O valor de H (height) da janela de projeção é obtido através do ângulo de vista da câmera e W (width) é obtido pelo produto de H pela razão de aspecto, para que assim, a ilustração do volume de vista passe uma ideia do que é possível enxergar através daquela câmera.
A2: Feito. 
      Obs.: Se durante o mode de renderização a câmera corrente for anulada, o modo editor é selecionado automaticamente. 

A3: O preview foi implementado fazendo-se uso das funções glViewPort() e glScissor() e possui a mesma razão de aspecto da câmera do objeto corrente que a possui.

A4: Foi implementado na classe P2 um método focus() que cria um botão na janela Hierarchy para focar no objeto de cena corrente.
Para fazer a câmera do editor focar no objeto de cena corrente, apenas tomou-se sua posição e transladou-se a câmera até esta posição. Para a câmera não ficar dentro do objeto, esta é afastada em 10 unidades para "trás". Mudanças na escala do objeto ou na rotação dos eixos da câmera não foram tratados.
O método é chamado no método hierarchyWindow() e, conforme a descrição da implementação, não altera a orientação dos eixos do VRC.

A5: Feito.

A6: Feito.
     Obs.: A adição de uma câmera a coloca dentro do objeto de cena a quem ela pertence. Como a câmera é um componente do objeto de cena, esta não possui transform próprio, logo, não é possível mexê-la livremente nesta implementação e, portanto, esta câmera não é muito útil no momento.