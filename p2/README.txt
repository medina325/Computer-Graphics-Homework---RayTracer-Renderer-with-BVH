Trabalho - P2 - Computa��o Gr�fica

Autores:
Gabriel Medina Braga (medina_cdz@hotmail.com)
M�rio de Ara�jo Carvalho (mariodearaujocarvalho@gmail.com)

Descri��o: 
Todas as atividades foram realizadas. 

A1: O m�todo drawCamera(c) foi implementado na classe SceneEditor. Os valores de F (near) e B (far) s�o obtidos da c�mera, por�m, como o intuito � ilustrar o volume de vista, os valores de F e B s�o multiplicados, respectivamente, pelos valores 54.00 e 0.003. O valor de H (height) da janela de proje��o � obtido atrav�s do �ngulo de vista da c�mera e W (width) � obtido pelo produto de H pela raz�o de aspecto, para que assim, a ilustra��o do volume de vista passe uma ideia do que � poss�vel enxergar atrav�s daquela c�mera.
A2: Feito. 
      Obs.: Se durante o mode de renderiza��o a c�mera corrente for anulada, o modo editor � selecionado automaticamente. 

A3: O preview foi implementado fazendo-se uso das fun��es glViewPort() e glScissor() e possui a mesma raz�o de aspecto da c�mera do objeto corrente que a possui.

A4: Foi implementado na classe P2 um m�todo focus() que cria um bot�o na janela Hierarchy para focar no objeto de cena corrente.
Para fazer a c�mera do editor focar no objeto de cena corrente, apenas tomou-se sua posi��o e transladou-se a c�mera at� esta posi��o. Para a c�mera n�o ficar dentro do objeto, esta � afastada em 10 unidades para "tr�s". Mudan�as na escala do objeto ou na rota��o dos eixos da c�mera n�o foram tratados.
O m�todo � chamado no m�todo hierarchyWindow() e, conforme a descri��o da implementa��o, n�o altera a orienta��o dos eixos do VRC.

A5: Feito.

A6: Feito.
     Obs.: A adi��o de uma c�mera a coloca dentro do objeto de cena a quem ela pertence. Como a c�mera � um componente do objeto de cena, esta n�o possui transform pr�prio, logo, n�o � poss�vel mex�-la livremente nesta implementa��o e, portanto, esta c�mera n�o � muito �til no momento.