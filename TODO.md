# To Do:

## Até 10-05

- [x] Adicionar modelos nossos - Caetano
- [ ] Texturização
  - [ ] Objetos específicos (asteróides) - Luiz
  - [x] Background - Luiz

## Até 17-05

- [ ] Câmera
  - [ ] Projeções - Caetano
  - [x] Troca de TPV para FPV - Luiz

## Se der, mas pronto para entrega final

- [ ] Iluminação
- [x] Curva de Bezier
- [ ] Colisões

## Pendente / Erro de Build
- [ ] Corrigir erro de build na classe Asteroid:
  - Mover `m_AsteroidCurveAngle` de `Application` para cada instância de `Asteroid`.
  - Cada `Asteroid` deve atualizar seu próprio `m_CurveAngle` no `Update`.
  - Fornecer um getter readonly para `m_CurveAngle` se necessário (embora o `Application` deva apenas chamar `Render`).
  - Garantir que `Asteroid::Update` calcule a matriz de modelo e a armazene internamente.
