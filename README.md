# CausticConstruction

![](https://raw.githubusercontent.com/Ydot12344/CausticConstruction/master/wikiRes/ezgif-5-cf5b5c4b47.gif)

**RU**: С++ OpenGL 3D рендер каустик на дне водоема.

**EN**: C++ OpengL 3D renderer of caustics on bottom of the reservoir.

## Render of water
### Wave generation
**RU**: Поверхность воды - это регулярная квадратная сетка размером 512 на 512. Высоты вершин хранятся в отдельной генерируемой текстуре - height-map. Чтобы понять координату вершины сетки в пространстве берутся координаты вершины и к высоте прибавляется соответственное значение в height-map.

В каждый пиксель height-map хранит 4 float значения RGBA. В R-канале мы храним высоту точки, в G-скорость данной точки по вертикали, в BA хранится xy составляющие нормированного вектора нормали к данной точке.

Обновление текстуры height-map:


## References
**RU**: Данная работа была разработана опираясь на следующие статьи:
* [Курс по OpenGL](https://habr.com/ru/post/310790/)
* [Статья о рендеренге каустик](https://habr.com/ru/post/520928/)
