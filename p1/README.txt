Trabalho - P1 - Computa��o Gr�fica

Autores:
Gabriel Medina Braga (medina_cdz@hotmail.com)
M�rio de Ara�jo Carvalho (mariodearaujocarvalho@gmail.com)

Descri��o: 
Todas as atividades foram realizadas. 

A1: Para implementar a cole��o de objetos da hierarquia de objetos de cena, foi utilizada a Classe Template std::list. Essa escolha foi tomada apenas para simplificar a implementa��o da hierarquia.
	Como era necess�rio que cada objeto de cena conhecesse tanto seus filhos como seu pai, a implementa��o da std::list como uma lista duplamente encadeada supriu esta necessidade, al�m de j� prover iteradores bidirecionais para acessos futuros. 

A2: A implementa��o de SceneObject::setParent(parent) foi razoavelmente intuitiva. Primeiramente, verificam-se duas possibilidades: "this" possui um pai ou n�o; se "this" n�o possuir um pai, este simplesmente toma seu parent como seu novo pai (que est� sendo passado por par�metro); e se "this" possuir um pai, ent�o ele � adicionado � lista de filhos de parent, para ent�o ser removido da lista de filhos de seu antigo pai e por fim, seu antigo pai � substitu�do por parent.
	 Uma observa��o a ser feita, � que por ter sido utilizada a std::list para implementar as listas, qualquer m�todo de remo��o de elementos (erase, remove, remove_if) destr�i o elemento removido, assim, � necess�rio que "this" seja adicionado � lista de filhos de parent antes de ser removido da lista de filhos de seu antigo pai.  

A3: A std::list tamb�m foi usada para implementar a cole��o de componentes de um objeto de cena. No momento em este relat�rio foi escrito, notou-se a desnecessidade de ter-se um lista duplamente encadeada para este caso, por�m, devido ao tempo e por este ser apenas um exerc�cio, esta escolha foi mantida.

A4: Foram criadas 2 n�veis de hierarquia em P1::buildScene(), contendo tanto objetos vazios quanto caixas. Para manter a coer�ncia na ordem de cria��o de cada objeto de cena, isto �, primeira caixa ser nomeada Box 1, a segunda como Box 2, o primeiro objeto vazio de Object 1, etc; foram concebidos dois atributos do tipo inteiro a classe P1: _emptyCount e _boxCount. Assim, basta contatenar o nome do objeto de cena com o inteiro convertido para string, e por fim converter para const char*.
	Tamb�m foram deixados comentados tr�s trechos de c�digo que demonstram a remo��o de objetos de cena da raiz da cena, filhos e redefini��o de um pai de um objeto de cena.

A5: Tanto a hierarquia quanto a funcionalidade do bot�o Create foram implementadas com sucesso.

A6: Como cada componente deve ter uma fun��o pr�pria para mostr�-lo em Inspector, foi criada uma fun��o an�loga a TransformEdit(transform), PrimitiveEdit(primitive). Esta fun��o mostra o n�mero de v�rtices do primitivo associado com um objeto de cena.

A7: P1::render foi implementado seguindo uma ideia do professor, atribuir uma lista de primitivos (a serem renderizados) para a classe Scene; como cada primitivo sabe quem � seu "dono", isto torna poss�vel apenas iterar pela lista de primitivos a fim de renderiz�-los. Por um lado, esta implementa��o � melhor que iterar pela lista de objetos de cena verificando se estes tem primitivo e ainda para cada objeto, iterar recursivamente nas suas respectivas listas de filhos; por�m, quando um objeto de cena � removido, o mesmo precisa ter seu primitivo retirado da lista de primitivos a serem renderizados.