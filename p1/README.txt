Trabalho - P1 - Computação Gráfica

Autores:
Gabriel Medina Braga (medina_cdz@hotmail.com)
Mário de Araújo Carvalho (mariodearaujocarvalho@gmail.com)

Descrição: 
Todas as atividades foram realizadas. 

A1: Para implementar a coleção de objetos da hierarquia de objetos de cena, foi utilizada a Classe Template std::list. Essa escolha foi tomada apenas para simplificar a implementação da hierarquia.
	Como era necessário que cada objeto de cena conhecesse tanto seus filhos como seu pai, a implementação da std::list como uma lista duplamente encadeada supriu esta necessidade, além de já prover iteradores bidirecionais para acessos futuros. 

A2: A implementação de SceneObject::setParent(parent) foi razoavelmente intuitiva. Primeiramente, verificam-se duas possibilidades: "this" possui um pai ou não; se "this" não possuir um pai, este simplesmente toma seu parent como seu novo pai (que está sendo passado por parâmetro); e se "this" possuir um pai, então ele é adicionado à lista de filhos de parent, para então ser removido da lista de filhos de seu antigo pai e por fim, seu antigo pai é substituído por parent.
	 Uma observação a ser feita, é que por ter sido utilizada a std::list para implementar as listas, qualquer método de remoção de elementos (erase, remove, remove_if) destrói o elemento removido, assim, é necessário que "this" seja adicionado à lista de filhos de parent antes de ser removido da lista de filhos de seu antigo pai.  

A3: A std::list também foi usada para implementar a coleção de componentes de um objeto de cena. No momento em este relatório foi escrito, notou-se a desnecessidade de ter-se um lista duplamente encadeada para este caso, porém, devido ao tempo e por este ser apenas um exercício, esta escolha foi mantida.

A4: Foram criadas 2 níveis de hierarquia em P1::buildScene(), contendo tanto objetos vazios quanto caixas. Para manter a coerência na ordem de criação de cada objeto de cena, isto é, primeira caixa ser nomeada Box 1, a segunda como Box 2, o primeiro objeto vazio de Object 1, etc; foram concebidos dois atributos do tipo inteiro a classe P1: _emptyCount e _boxCount. Assim, basta contatenar o nome do objeto de cena com o inteiro convertido para string, e por fim converter para const char*.
	Também foram deixados comentados três trechos de código que demonstram a remoção de objetos de cena da raiz da cena, filhos e redefinição de um pai de um objeto de cena.

A5: Tanto a hierarquia quanto a funcionalidade do botão Create foram implementadas com sucesso.

A6: Como cada componente deve ter uma função própria para mostrá-lo em Inspector, foi criada uma função análoga a TransformEdit(transform), PrimitiveEdit(primitive). Esta função mostra o número de vértices do primitivo associado com um objeto de cena.

A7: P1::render foi implementado seguindo uma ideia do professor, atribuir uma lista de primitivos (a serem renderizados) para a classe Scene; como cada primitivo sabe quem é seu "dono", isto torna possível apenas iterar pela lista de primitivos a fim de renderizá-los. Por um lado, esta implementação é melhor que iterar pela lista de objetos de cena verificando se estes tem primitivo e ainda para cada objeto, iterar recursivamente nas suas respectivas listas de filhos; porém, quando um objeto de cena é removido, o mesmo precisa ter seu primitivo retirado da lista de primitivos a serem renderizados.