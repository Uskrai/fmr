# fmr
Free Manga Reader

<h2><b>Requirement</b></h2>
<ul>
  <li>wxWidgets >= 3.0.4</li>
  <li>c++17</li>
</ul>
<h2><b>How to build</b></h2>
<ol>
  <li>cd /path/to/sources</li>
  <li>mkdir build</li>
  <li>../configure</li>
  <li>make -j3</li>
</ol>
<h2><b>Option</b></h2>
<span>--with-wx-config=/path/to/wx-config     // specify wx-config path</span><br>
<span>--disable-shared or --enable-shared=no  // make static link</span><br>
<span>--build=x86_64-w64-mingw32 --host=x86_64-linux // cross-compile</span><br>
