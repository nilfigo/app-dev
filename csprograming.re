﻿= 作って学ぶ　はじめてのC#

実際に、コードを書きながら、小さめの（デスクトップ）アプリを作ってみる。その過程で使うこと、その他を一通り網羅しているつもりである。

== はじめに

プログラミング初心者（＝自分）に向けた、超絶丁寧な(冗長な)解説書を作りたいと思ったので書く。
エラー、コードミス等も説明することで、どんな間違いが起こるのか、なぜ起こるのか、といったところまで説明したい。

なお、本章は、エレガンスなコーディングを求める本ではないし、おそらく実装効率は悪い。きちんとクラス化・モジュール化することで、可読性が高く、修正が少ないコードが書けるはずであることは重々承知している。しかし、それはそれでテクニックとして高度で、参照やモジュール化に必要な手続き、実装が増えるため、初心者には分かりづらい「おまじない」となってしまう可能性が高い。そこで、とりあえず動く！を基本として、できるだけ一本道でコーディングすることを目的としたい。

もちろん、経験者にとっては、ヘボいコード、汚い、など、拙いものであることはわかる。が、初心者が躓く(著者が実際に躓いた。ので、おそらく他の人も躓く)点であろうことから、温かい目で見守っていただければと思う。リファクタリングを含めた改善は今後、もっとプログラミングの腕が上がってからでよいと考える。

俺たちは雰囲気でプログラミングしている！

== 作るものを決める

何はともあれ、作るものを決めなければ、コーディングを始める事はできない。一般にプログラミングの教科書に載っているコーディング例は、Hello！World！の次は、なんだかよくわかない、実用性もないプログラムであって、作るモチベーションがわかない。また、クラスがどうの、という説明が長々と書かれていたりして、辟易するものである。数値の型なども深堀りし始めるときりがないので、都度必要に応じて説明することにして、インデックスなどの明示的に整数なものはInt(32）、データを格納する部分は一旦は全部浮動小数点（double)で定義する。

そこで、本章では、少々規模は大きくなるものの、ある程度実用性のある「データ解析ツール」なるものを作ってみることにする。本業が、大規模プラントにおける計測・制御の研究開発をやっている関係で、多数のデータを取って、そのデータを処理していくというのはよく直面する。ところが当然ながらデータは案件ごとに異なる。当然データの構造も違えば、行いたい解析は異なる。データの解析をいちいちエクセル等で実行するのは非常に面倒だし、マクロを作るのも難しいので、解析待ちの積みデータが増える。もっとも、マクロもプログラミングの一種であるから、エクセルVBAで作るのと、個別のプログラムを作ることに大差はないかもしれない。

データ解析がかったるいと、積みデータが増え、せっかく取ったデータが腐るし、開発スピードも低下する。

そこで、自分が必要なデータを読み込み、前処理し、必要な演算を行い、保存用データとして出力するとともに、グラフ化、画像化するツールを作る。

ここで扱う機能は、ファイルの入出力、四則演算、画像生成、マウスによる操作・値抽出など、プログラミングの基礎的な内容はだいたい盛り込まれているし、自分で作るツールであるから、自分が必要な機能を簡単に実装することができる。

また、一度ベースとなるソフトを作っておけば、似たような案件、次の別の案件においても小改造で活用することができ、次案件の着手速度がUPする。また、グループでの共有も可能になり、グループにおける秘伝のタレとなることも期待できる。実際自分の所属するグループでそうなっているコードもあるのだが、たとえば開発環境がレガシーだったり、元の作者の癖があったりで読みづらいなど、すぐに使うのは少々しんどい状況にあったりするので、自分の秘伝のタレを作っておくことが望ましい。

=== 作るものの細かい機能を羅列する。

まだソフト(Visual Studio等)を起動する必要はない。必要なのは紙と鉛筆である。これから作るソフトの設計を行う。要件仕様と読み替えてもよいかもしれない。解析元のデータをもとに、盛り込みたい機能を列挙し、ユーザインターフェースを考える。どんな画面が必要か、グラフはどこに置くか、などを考える。改めて言うが、ここである程度必要な機能を考えてからコーディングに入る必要がある。もちろんコーディング途中に機能が追加されることもあるだろうが、それはもちろんOKである。

それでは、具体的に設計に入ろう。

まずは、取得した、あるいは取得する予定のデータを用意する。縦のデータ点数(時系列)、横のデータ点数(取得回数)を考える。
例えば、温度計測の生データ(センサ出力電圧をAD変換(Analog to Digital変換)し記録した時系列データとする。
そのデータが、例えば1分に1回のサンプリングで、1日1ファイル、100日分あるとする。
この元データがどういうものであるのか、バイナリファイルなのか、CSVファイル、テキストファイルなのか、で若干読み込み部分が異なる。また、これからデータ取得ソフトを製作するなら、その要件定義にも関わるので、ここで定義をしっかり固めておくと手戻りがない。

====[column] 測定対象とサンプリング間隔

ここでの工夫としては、「取れるものは全部取っておく」が基本である。もちろんサンプリング間隔やハードの点数の問題もあるが、例えば、測定時刻、生データのみならず、測定回数のインクリメント、測定開始からの経過日数等も準備しておくと、このあとの利用や解析が楽になることが少なくない。

例えば、測定回数のインクリメントをデータ中に保存しておくと、データを間引くときに、10回に1回にしよう、といったときに楽になる事があるからである。もちろん、生のデータから数えて間引く判定をするというのも可能ではあるが。

とはいえ、真逆のことを言うようだが、データ点数をむやみに増やすと、データ量が爆発することになる。必要のないデータを取らないようにする必要はある。

温度計測を例に考えた場合、データの意味するところを考え、適切なサンプリング間隔を設計する必要がある。同じく温度データであるが、測定対象および現象によって必要なサンプリング間隔が当然異なるのである。

温度計測の中でも、室温計測という例を考えてみよう。「エアコンの効きが悪い気がするので、温度を測定してみよう。」といった状況を考えると。1秒に1回程度のサンプリングが適切である。エアコンOFFの状態から、数分で温度が落ち着くはずである。そこで、例えば1秒おきに温度変化を取得するのが適切であろう。

これに対して、可能だからといって、1msでサンプリングする必要はない。同じデータがずっと続くだけなので、1ms後の温度指示値に意味はないからである。逆に、エアコンによる温度変化を記録するのに、1時間ごとに測定しても意味がない。5分ほどで気温は落ち着くからである。

また、刀鍛冶の焼入れを想定してもらえればよいが、熱したものを水に浸けて冷却する瞬間の温度変化を見たい、といった場合には、1ms周期、あるいはもっと高速なサンプリングが必要になるかもしれない。逆に、ヒートアイランド現象について調査するために室温の変化をモニタリングしたい、といった場合には、1分ごとでもデータ過多で、10分とか1時間ごとのデータ取得で十分かもしれない。

サンプリング定理などでググってみると、適切な測定周期について設計の一助になるかもしれない。

====[/column]

さて、いずれにせよデータは取れたものとする。
これらのデータを一括で読み込み、電圧を温度に変換し、まとめデータとして編集する。
また、一日の温度変化と異常点を可視化するために、コンター図に変換する。
その後、その結果を出力する機能を持つものとする。

次に、以上の機能を、もう一段階細かい機能に落とし込む。基本的には、データ処理の流れを可視化、整理する。それぞれのステージにおいて実装したい機能を明示しておくと、コーディングが楽になる。それぞれの関数として切り分けるなども可能になるので、箇条書きで機能を列挙してみる。

 * ファイル読み込み
 * データ前処理
 * 描画
 * ファイル出力

という機能が最低限必要である。次に、これをさらに書き下す。

 * ファイル読み込み
 ** CSVファイルを(拡張子指定で)開く
 ** 複数ファイルを選択可能にする。
 ** テンポラリ配列に格納
 * データ前処理
 ** 電圧→温度変換
 ** 平均化
 ** 縦軸時間×日の二次元配列(リスト)に格納
 * 描画
 ** 描画領域を確保する。
 ** メイン画面に二次元データを表示する
 ** サブ画面1に、1日分の連続グラフを表示する。
 ** サブ画面2に、毎日同時刻におけるデータを表示する。
 ** 日・時刻の選択はマウスでクリックしたところ。
 ** 画面のスケーリングを行う(見やすく調整する)
 ** 画像として保存する。
 * ファイル出力
 ** 保存したファイルを、CSVで保存する。（またはバイナリデータでも良い)
 ** バイナリファイルの読み込み機能をつける。

とりあえず、これくらいの機能を実装する。



=== 実装の方法論について

実装方法にも当然様々あるが、そこら辺勉強したことがないのと、それぞれの方法についての名前もよくわからないので、実装方法についての解説はしない。

基本的に、使用者は自分で、データの流れに沿って計算していくだけのツールなので、演算自体は一本道である。あるデータをうけてこの演算を行う、とか、この操作をしたらこう表示する、といった程度のものである。また、対象となるデータは固定であるから、処理に柔軟性をもたせる必要がないので、一旦ハードコーディングを基本とする。

ハードコーディングとは、計算に必要な数値(係数など)を直接ソースコードに書くことである。例えば、商品単価から支払額を算出する場合に、消費税(8％)を、ソースコード中に*0.08と記入するようなものである。

ハードコーディングの問題は、例えば将来増税または減税等で税率が変更された場合に、この数値がでてくる部分をすべて修正する必要がある。係数の変換ごときで再コンパイルが必要になるのは、ダメな設計であることは明らかである。

ところで、ハードコーディングの対義語は？と探してみたが、見つからなかった。ハードの対義語としてソフトを選び、ソフトコーディング、とかんがえてみたが、それは実装そのものをさすものであり、ハードコーディングの対義語にはなりえない。

さて、ハードコーディングを避ける方法として、ソフト上に当該定数を入れるフィールドを作る、外部の設定ファイルを参照するなどの方法があるが、洗練されたコードを書くのはとりあえずあと回しとし、シンプルなコーディングを旨とする。

随所からマサカリが飛んでくる可能性は十分承知しているが、ハードコーディングする理由を述べる。計算に使う係数を外部ファイルを参照するなどすると、その定義ファイルを書き換えるだけで修正できるので、便利であることは明らかである。しかし、極論すれば、×2を計算するために、x2という係数を記録した設定ファイルを読み込み、指定のフィールドから読み取り、文字列を数字に換算して、係数とする必要があるなど、必要な手数が大きく増加する。またそのファイルが見つからない場合にはエラーになるし、設定ファイルが見つかりませんエラー処理が必要になる。プログラミング初心者にとって、「直ちに意図がつかめないおまじない」の存在は、つまづきの原因となる。しかも、このハードコーディングを避けるためのおまじないは、自分で意味がつかめる数値のハードコーディングとは異なり、ファイルを読み込むためのよくわからないコードや中身のわからない変数列となり、全コードの網羅理解を妨げる。

このように、ハードコーディングすればA=a*2;というたった１行のコードで済むところが、係数を変数で定義し、係数を記録したファイルを定義し、ファイルを読み込み、係数を探し、係数を変数に代入し、ようやくA=a*2が得られる。という実装が必要になる。コード量が増え、理解するのに時間がかかる。係数が頻繁に変わったり、個数が多いなどの場合は当然それに見合った対応が必要になるが、ハードコーディングすればシンプルで済むという意図を理解してもらえると思う。

同じ理由から、類似のコードをクラス・メソッドとして独立させることも、あと回しにする。同じ内容のコードを繰り返し書く必要があったり、修正に漏れが生じるなどのリスクは承知した上で、メソッドを動かすための引数、その参照範囲など、これまた理解し辛い概念が多数でてくるからである。

また、エラー処理も貧弱である可能性が極めて高い。少し想定と違うファイルを選択したらいきなりException(例外)を吐いてソフトが落ちる、といったこともあり得るし、操作順によってはエラーとなる可能性も多分に存在する。先に述べたように、一本道のコーディングなので、それから外れた操作については極めて脆弱である。

例えば、ファイルを読み込んで、ボタン1を押すとAの計算を行い結果aを出し、ボタン2を押すとaにBの計算を行い結果bを出し、ボタン3を押すとbにCの計算を行い結果cを出すという流れがあるとする。このとき、いきなりボタン2を押すと、aが存在しないのでエラーが起こるのだが、そのエラーを検知・回避する手当が必要になる。あるいは、異常な値のまま警告なく次に進んでしまう、といったことが起こり得る。キチンと作ったソフトならばそのあたりは当然抜けなく対処すべきであるが、一旦はそこまで求めないこととする。

=== 画像化の威力について

データ処理において、画像の威力は非常に大きい。時系列データを可視化するとき、グラフを作ることは多いが、数値を輝度とした2次元画像を作ることはデータの評価上非常に優位である。

これは、人の目、認識がパターン抽出に極めて優れているからである。信号処理の領域において、SN比(Signal to Noise Ratio)という指標がある。信号とノイズの比で、信号が小さく、ノイズが大きい場合は信号の有無判別が困難となる。

具体的な信号の例を使って説明する。まず、ノコギリ波状の信号を考える。この信号は一定周期で発生しており、タイミングずれ（ジッタ）はないものとする。まず、ノイズのない波形を@<img>{SignalNoNoise}に示す。

//image[SignalNoNoise][ノイズなしの信号]

これに対し、実際の測定では多かれ少なかれノイズが混入する。

１点置きに振幅の１００％のノイズを混入させた例を、@<img>{SignalNoise}に示す。
//image[SignalNoise][ノイズありの信号]

もともとの波形を知っていれば、あるいは心の目で50～75付近に信号があることがわかるが、非常に大きなノイズに埋もれていて、ほとんど見分けがつかない。

これに対し、この信号が繰り返し生じることを用いて、画像化することを考える。

ここでは、二次元画像の横方向に一つの信号を表し、繰り返しを縦方向に取る。一つの信号の信号レベルをモノクロの輝度値に変換する。生信号が0～２５５の８bit、輝度信号も、RGBの３値を同一値としてグレースケール化する。この場合、信号の値がそのまま画素輝度となる。

２００個のデータについて、画像化したものが、@<img>{NoNoize}である。はっきりと帯状の信号が確認できる。

//image[NoNoize][ノイズなしの二次元画像]

次に、ノイズはランダムに発生するため、ノイズなしの信号の１点おきに振幅の１００%のノイズを混入させた信号を生成し、画像化したものを@<img>{WithNoize}に示す。

//image[WithNoize][ノイズありの二次元画像]

図より明らかなように、（ちょっとズルはしているが）非常に強いノイズの混入に対しても、信号の識別が非常に容易になる。

これが、画像化の威力である。

本来は生データ+ランダム信号で信号生成をしたかったが、入稿日との兼ね合いで突貫かつズルをしてしまったことをお詫び申し上げる。ただし、この考え方自体は嘘ではないので、ご容赦いただきたい。

なお、本画像生成のために使ったコードを、@<list>{make2DMap}に示す。前半でノイズなし波形を生成し、後半んでランダムノイズで上書きしている。

//listnum[make2DMap][二次元信号の画像生成コード]{
Bitmap canvas = new Bitmap(pictureBox1.Width, pictureBox1.Height);
for (int x = 0; x < 50; x++)
{
    for (int y = 0; y < pictureBox1.Height; y++)
    {
        Color gladC = Color.FromArgb(0, 0, 0);
        canvas.SetPixel(x, y, gladC);
    }
}
for (int x = 50; x < 75;x++ )
{
    for (int y = 0; y < pictureBox1.Height; y++)
    {
        int xplot = (x - 50)*10;
        Color gladC = Color.FromArgb(xplot,xplot,xplot);
        canvas.SetPixel(x, y, gladC);
    }
}
for (int x = 75; x < 200; x++)
{
    for (int y = 0; y < pictureBox1.Height; y++)
    {
        Color gladC = Color.FromArgb(0, 0, 0);
        canvas.SetPixel(x, y, gladC);
    }
}

Random r= new Random();
for(int x =0;x<200;x+=2){
    for(int y=0;y<200;y+=2){
        int randomColor = r.Next(255);
        Color rand = Color.FromArgb(randomColor,randomColor,randomColor);
        canvas.SetPixel(x,y,rand);
    }
}
pictureBox1.Image = canvas;

SaveFileDialog sfd = new SaveFileDialog();
sfd.Filter = "bmp|*.bmp";
if (sfd.ShowDialog() == DialogResult.OK)
{
    canvas.Save(sfd.FileName);
}
//}


== 事前準備

事前準備として、Visual Studio Express2015と、Sourcetreeの利用を前提とする。

=== VisualStudioのインストール

VisualStudioはMicrosoftの提供する統合開発環境であり、Visual C++、Visual C#、VisualBasic等が使用できる。環境構築済みの方は本節は読み飛ばしていただいて構わない。

特段、バージョンに依存するコーディングはないはずであるが、本書の環境はVisualStudio2015を前提としている。単に筆者の環境がそうだった、というだけで、これからインストールする方はあえて古い環境を使う必要もないので、最新の2017Communityを利用すれば良いと思われる。若干UTなどが異なるかもしれないが、実装については大きく異なるものではないと考えられる。もしあったとして、適宜読み替えていただければと思う。

さて、Visual Studio Expressは、ユーザー登録すれば無料で利用できる開発環境であり、一部機能は制限されているようである。また、MSDNのサブスクリプションはつかない。とはいえ、通常使うには必要十分な機能が備わっている。まずは、これをインストールする。インストーラーの指示に従ってインストールを進めれば良い。

なお、VS2015にはExpressがあるようだが、いつのまにかVisualStudio Communityなるエディションに切り替わっており、フリーで使えるようだが、いくつか制約と制限があるようである。
具体的には、Communityは商用利用はNGだったり、チーム開発には使えないなどの制約@<fn>{community}があるようである。仕事で使う場合などは気をつけていただきたい。Expressは商用利用もOKなようである。

//footnote[community][https://www.buildinsider.net/hub/insidersbreak/2014112101]

=== Gitを使う(Sourcetreeの超基本のところ)

次に、Sourcetreeをインストールする。Gitの説明については、「わかばちゃんと学ぶGit使い方入門」@<fn>{wakaba}をぜひ参照いただきたい。Gitの考え方や使い方について網羅的かつわかりやすく記載したとてつもなく素晴らしい本である。この本のおかげで、コーディングのバージョン管理という概念を得て、使ってみよう、さらにはこの本を書いてみようという気になったのである。

//footnote[wakaba][わかばちゃんと学ぶ Git使い方入門〈GitHub、Bitbucket、SourceTree〉湊川あい著　シーアンドアール研究所 (2017/4/21)　http://amzn.asia/fEgV1Wu ]

とはいえ、今回の開発においては諸般の事情からローカルでの利用のみとなるため、必要な部分のみを解説することにする。

また、Gitの様々な説明を見ると、コマンドラインでの説明も少なくない。が、上記の通り、わかばちゃんと学ぶGit使い方入門に基づいて実施するので、コマンドラインでのGitの使い方には一切触れず、Sourcetree使用を前提とすることをご了承いただきたい。

こちらもまずインストールして、使用環境を整える。インストールウィザードに従えばよい。AtrassianIDを作っておくと便利だと思われる。

インストールが終わったら、Sourcetreeを起動する。起動直後の画面で、何もなければ新しいTabのみが表示されているはずである。NewTab内で、”Create+”をクリックする。

//image[Sourcetree1_openNewTab][Sourcetree起動画面]

//image[Sourcetree2-2_CreateNewRepository][新しいリポジトリを作る]
//image[Sourcetree2_CreateNewRepository][新しいリポジトリを作る（ローカルのみ）]

この時、リモートリポジトリも併用するなら、リモートの情報を使うが、ローカルリポジトリのみで実施するなら、次のアカウントでリポジトリを作るのチェックを外して構わない。

ここで、Visual Studioを起動して、プロジェクトを作ってしまうとよい。

//image[Sourcetree3_SelectFolder][VisualStudioのプロジェクトフォルダを指定]

保存先のパスに直接入力または参照をクリックし、先ほど作ったVisual Studioのプロジェクトのパスを指定し、作成をクリックする。

//image[Sourcetree4_AlreadyExistFolder][すでにあるフォルダを指定したとき]

すでにあるフォルダをGitリポジトリにする場合、@<img>{Sourcetree4_AlreadyExistFolder}のような確認が出るようである。ここはOKを押して問題ない。

すると、@<img>{Sourcetree5_StartWatching}のような画面になり、ファイル更新の追跡が始まる。ここでは、VisualStudioが生成したファイルすべてが新規ファイルとして検出されている。

//image[Sourcetree5_StartWatching][ファイル追跡開始画面]

この状態を記録し、次の変更を追跡するため、ステージングする。インデックスに入れるをクリックすると、上段の「indexにステージしたファイル」欄にファイルが移動する。

//image[Sourcetree6_index][ステージング済の状態兼コミット待ち]

ステージング済の状態は、コミット待ちの状態でもあるため、@<img>{Sourcetree6_index}のように、コミットコメント欄が出る状態になる。

コミットコメント欄に、例えば、「最初のコミット。VisualStudioによる自動生成コード」などと状況説明のコメントを入れ、コミットをクリックする。


//image[Sourcetree8_ConfirmCommit][コミット済みの画面（コミットログ画面)]

すると、@<img>{Sourcetree8_ConfirmCommit}のように、コミットログ（樹形図、ファイル変更履歴等)の画面に遷移する。

次に、Visual Studioまたは適当なエディタで、リポジトリ内のファイルを編集してみよう。適当なファイルに、コメントを追加して保存した例を示す。変更したファイルと、変更箇所が@<img>{Sourcetree9_EditTest}のように表示されている。

//image[Sourcetree9_EditTest][ファイル編集の捕捉テスト]

追加した行は薄緑で、削除した行はピンクでハイライトされ、変更箇所がわかるようになっている。また、編集は、行の追加と削除が並行して行われたと認識されるので、ピンクと薄緑の行が並んで存在することになる。

監視対象フォルダをプロジェクトのフォルダとした場合、コンパイルによって生成する中間ファイルなどがコンパイルのたびに記録されることになる。これはこれでうっとおしいので、監視したくないリストに入れておくとよい。.gitignoreという設定ファイルがあるので、ここに追記することで監視対象から外すことができる。基本的にはコードのみを監視対象とするので、下部フォルダ（binなど)を除外ファイルに指定しておくとよい。

== コーディング開始。

=== はじめはHello World

まずは、VisualStudioを起動し、@<img>{VSStart}のようにVisualC#を選択、新規Windowsプログラムを選択する。

//image[VSStart][C#で新規プロジェクトを選ぶ][scale=0.5]

のっぺらぼうな画面が一つ出るはずである。これが新規プロジェクト起動時の画面である。ここからコーディングが始まる。

//image[VSnewWindow][新規プロジェクトを作成・起動した状態]

ここに、ツールボックスからボタンを選択し、配置する。ドラッグすれば適当なサイズに変更できる。
プロパティから、オブジェクト名：btnOpenCSV
Text:OpenCSVと入力する。

命名規則は個人によりけりだと思うが、コントロールの種類（ボタンなのか、テキストボックスなのか、ラベルなのかチェックボックスなのかなど）がひと目で分かり、かつそれが何をするものなのか。（ボタンであれば、ファイルを開くボタンなのか、終了するボタンなのか、など）がわかるように、名前をつけることが望ましい。今回のボタンは、csvファイルをひらくボタンであるから、btnOpenCSVと名前をつけた。なお、筆者は、できるだけ以下のように名前を付けるようにしている。
 
 * ラベル：lbl　温度データ表示用ラベル＝lbltemperature
 * テキストボックス：tb 現在時刻表示ボックス＝tbTimeNow
 * ボタン：btn 開始ボタン＝btnStart
 * チェックボックス：cb 

次に、Form1.cs[デザイン］のタブにある画面を右クリックし、コードの表示をクリックする。

//listnum[code1][自動生成されたコード]{
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace csvAnalyzer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
    }
}
//}

というコードが自動生成されているはずである。ここに必要なコードを順次記入していく。
もちろんそれぞれの行に意味はあるのだが、ここではあまり深入りはしない。

ごく簡単に説明しておくと、最初のusingから始まる数行は、よく使用するライブラリを事前に読み込んでおくものであり、基本的な機能であれば特に気にせずいきなり書いても使えるために必要なものである。ここに書かれていないライブラリを使う場合などは、これらに続けて記載する。C++などではIncludeと呼ぶので、「xxというライブラリをIncludeせよ」と書いてあれば、この要領で登録する。確かGUIでもできたと思うが、今回は追加でincludeするものは特にないので、このまま実装に進めばよい。次のNamespace（名前空間）の宣言により、一連のコードが属する範囲を定めている。変数や関数など、このNamespaceに属するものを明確にするためである。そして、さらにその中に、Form1というに属するものがあるという入れ子構造となる。

様々な変数や関数は、Form1で使うので、今後のコードは、 public partial class Form1 : Form { }
の中に記述していくことになる。ウィンドウが増えるなどの場合はそれに応じて参照を適切に処理する必要がある。

さて、デザイナータブに戻り、先程置いたボタンをダブルクリックする。すると以下のコードが自動で追記される。名前の通り「ボタンをクリックしたとき」に｛｝内の処理をしなさい。という意味である。

//listnum[code2][ユーザーイベント時のコード]{
        private void btnOpenCSV_Click(object sender, EventArgs e)
        {
            //ここに書くんですよー
        }
//}

そこで、{}の間に、このアプリを終了する関数を記入する。

//listnum[codeExit][ユーザーイベント時のコード]{
        private void btnOpenCSV_Click(object sender, EventArgs e)
        {
            Application.Exit();　//アプリケーション終了を実装。
        }
//}

次に、VSのツールバーにあるDebugボタンを押して、コンパイルする。エラーがなければ、２-３秒すると、ウィンドウが立ち上がるはずである。そして、ボタンをクリックすると、ソフトが終了するはずである。

これで初めてのプログラムができた。おめでとうございます。この瞬間に、C#でプログラムを書いて、動かした、という実績がアンロックされたことになる。

一応Hello Worldもやっておくことにしよう

//listnum[code3][HelloWorldのコード例]{
Console.Write("HelloWorld");
MessageBox.Show("HelloWorld!","HW!!",MessageBoxButtons.OK,MessageBoxIcon.Error);
    //ウィンドウ内テキスト、キャプション、表示するボタン（OKのみ）、エラーアイコン
//}

実行すると、出力(コンソール)に、HelloWorldと表示されるはずである。

なお、@<code>{Console.Write();}は、値や文字列を表示する事ができるため、読み取ったデータの確認、引数の確認等に利用する事ができる。例えば、ファイル読み込みのときに指定したファイルのパスがきちんと取得できているか、開いたファイルの先頭部分を表示して正しい操作が行われているか、計算途中の値を表示して、計算が正しく実装できているか、ループのカウントが正しいかなどの確認に利用できる。

あわせて、メッセージウィンドウを出す方法もここでテストしておく。
実行し、ボタンを押すと、HelloWorldと書かれたビックリマーク付きのポップアップウィンドウが表示されるはずである。

その他のメッセージボックスについては、例えば以下のURLを参照されたい。
https://dobon.net/vb/dotnet/form/msgbox.html

ここでのコツは、複数同じような関数を使う場合などには、例えばビックリマークの数を変えるとか、数値を入れるなど、どの行のコードがどの動きとして現れるかを確認できるようにしておくとよい。

なお、コンソール出力の一部を示す。
//listnum[code4][出力の例]{
HelloWorld
'csvAnalyzer.vshost.exe' (CLR v4.0.30319: csvAnalyzer.vshost.exe): 
プログラム '[6156] csvAnalyzer.vshost.exe' はコード 0 (0x0) で終了しました。
//}

ここまで動作が問題なければ、Hello Worldを終了する。

さて、ここでSourcetreeのほうに移動すると、インデックスされていないファイル、という画面に、２つないしは３つのファイルが表示されているはずであるから、インデックスに入れるをクリックし、ステージし、コミットする。コミットメッセージは、Hello!World！実装とでもしておけばよい。

コンパイルするときにソースコードが保存されるので、VisualStudioの方で明示的にファイル保存の手続きを取らずとも、ファイルが保存され、Sourcetreeの方で更新がキャッチされる。これをステージングし、コミットする。

お作法として、何を実装/修正したか、あるいは、必要に応じてなぜその追加・変更を行ったのかがわかるようにコミットメッセージを入れると良い。「どのように」変更したかはソースを見ればわかるが、「なぜ」変更したかは一般にはソースにはかかれないためである。

また、ステージングは細かい頻度で実施するのが良い。二つ以上の変更点を一つのステージングするのは避けた方が良いだろう。片側の実装が不要になったとか間違いがあったとかのときに対応が難しくなる。また特に実装がうまくいかず試行錯誤をするときは細かい頻度で実施するとよい。

また、こまめにステージングすることにより、小ブロックでの実装テストも可能になる。例えば、「ファイルを読み込む」という動作においても、OpenFileDialogでファイルを指定し、ファイルタイプを指定し、ファイルパスを取得し、ファイルを開き、中身を1行読み取り、複数行読み取るのを繰り返し、中身を配列に格納し、ファイルを閉じて、次の処理に移行する。このように、要件仕様では１行の機能でもコードは数行から数十行にわたり、細かく書き始めればキリがない。それぞれのステップでミスは発生しうるので、一行ごととは言わないまでも、ひとかたまりごとにコミットするくらいの癖をつけるのが良いと思われる。

一人開発ならばあまり細かいことは気にしなくても良いが、グループ開発ならば、Gitのお作法についてはぜひ注意されたい。少し世の中の基本を確認したり、グループのお作法についてもケアするとより治安が保たれるかもしれない。

=== ファイルオープン

データ解析ツールなので、元ファイルを開かないことには何も始まらない。
元データの形式は、次の様なものである。時間と温度センサの指示値が並んでいるものであるとする。温度センサの指示値を20倍したものが測定温度であるとする。
そこで、読み込むデータを以下の形式であるとする。

//listnum[datatable][読み込むデータ]{
Time Voltage(V)
2018/6/18 12:34:56 1.45
2018/6/18 12:35:06 1.46
・・・
//}

10秒毎計測で、1日1ファイル(8640行)である。

そこで、ファイルを読み込み、配列（とりあえずデータを入れる箱だと思って良い）に格納し、電圧→温度に変換する計算を行う。

ファイル読み込み部分のコードを実装する。

まず、ファイルを開いて読み取るコードである。

//listnum[opencode][ファイルオープン]{
OpenFileDialog openFileDialog1 = new OpenFileDialog(); 
    //新しいOpenFileDialogを作る
if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)	
    //OpenFileDialogがOKなら、新しいダイアログを開く
{
    string strFileName = System.IO.Path.GetFileName(strFilePath);
    listBox_filename.Items.Add(strFileName);    //ファイル名取得し、表示する。

    using (StreamReader sr = new StreamReader(strFilePath))　//読み込みツール準備
    {
        while (!sr.EndOfStream)　　//ファイルの終わりまで繰り返す
        {
    		sr.ReadLine();	　//１行づつ読み込む
        }
	}
}
//}
このコードでは、"読むだけ"である。どこにも格納したりしないので、一見なにも起こらない。そこで、読み込んだデータを表示するコードを追加する。

//listnum[readCode][読み込んで値を格納するコード]{
String line = sr.ReadLine();            //読み込んで、仮の文字列に格納。
string[] splitData = line.Split(',');   //コンマ(,)で分割して、文字列の配列に格納。
Double rawdata = Double.Parse(splitData[1]);
    //2列目めを取り出す。(ゼロオリジンなので、1) 取り出した文字列を数値に変換。                      
addDataArray[k] = rawdata;              //データ格納用配列(ここではaddDataに格納)
k++;                                    //格納用インデックスをインクリメント
//}

もし、ヘッダー行がある場合は、何もしないでその行数分だけ読み飛ばせば良い。

//listnum[skipcode][ヘッダー行を読み飛ばす]{
for (int i = 1; i <= 5; i++)
{
    sr.ReadLine();
}
//}

解析時の利便性のため、OpenFileDialogのオプションを設定する事ができる。必要ないファイルを表示しないようにしてファイルの指定を確実にする、といったこともできる。ここでは、名前が異なっている


//listnum[fileFilter][ファイルフィルターの設定]{
openFileDialog1.Multiselect = true;　//複数ファイル選択可能とする。
    //開くファイルの拡張子、ファイル名をフィルタする。
openFileDialog1.Filter = "CSV|*.csv|csv-X|*X.csv|AllFile|*.*";
    //生のcsvファイル、特定の名前付きcsvファイル、すべてのファイル
openFileDialog1.FilterIndex = 2;	//開くファイルの初期値設定=(2)
openFileDialog1.RestoreDirectory = true;　//以前開いたフォルダを記録する
//}

また、複数ファイルを選択した場合、foreachを用いて、開いたそれぞれのファイルに対して処理を行うように設定する。

//listnum[multifileloop][複数ファイルの選択可能化]{
foreach (string strFilePath in openFileDialog1.FileNames)
    {
	//ファイルオープン、読み取り、格納等の処理を書く。
	}
//}

=== ファイル名から日付を取り出す。

ファイル名を測定日時とすることは多いであろう。
例えば、2018年07月05日　12：23：34生成のファイルを、20180705122334.csvなどと命名する事がある。タイムスタンプから測定時刻を取得するのもよいが、ファイル名から取ってきた文字列を時間データに変換する方法@<fn>{filenametodate}について述べておく。

//footnote[filenametodate][http://www.atmarkit.co.jp/ait/articles/0409/03/news087.html]

基本的には、取得したファイル名(文字列：String)から、時刻(DateTime型)に変換してやればよい。このときには、Parseメソッドを使う。

また、もし、20180705122334Temp.csvなど、時刻以外の文字列が付加されているなら、それを整形する必要がある。Substringメソッドを使うと、文字列の指定の位置から指定文字数を取り出すことができる。

//listnum[filenametodate][ファイル名から日付への変換]{

string strFileName = System.IO.Path.GetFileName(strFilePath);
    //OpenFileDialogで取得したstrFilePathをstrFileNameに格納する。
string filenameastime;
filenameastime = strFileName.Substring(0, 14);
    //Substringで前から14文字を取り出す。開始点、終了点。
string f;
f = "yyyyMMddHHmmss"; //フォーマットを指定。年年年年月月日日時時分分秒秒
measureDate[filecount]=DateTime.ParseExact(filenameastime,f,null);
    //文字列filenameastimeをDateTimeに変換して、配列に格納。
lblDate.Text = measureDate[filecount].ToString();
    //DateTimeを文字列に戻して、日付表示用ラベルに表示
//}

また、ファイルのタイムスタンプを読み取る場合には、FileクラスにあるGetCreationTimeメソッドなどを用いると良い。

//listnum[getFileDate][ファイルのタイムスタンプを読み取る]{
//ファイル生成日時取得
Console.WriteLine(System.IO.File.GetCreationTime("C:\test.txt"));
//ファイル更新日時取得
Console.WriteLine(System.IO.File.GetCreationTime("C:\test.txt"));
//ファイルアクセス日時の取得
Console.WriteLine(System.IO.File.GetLastAccessTime("C:\test.txt"));
//}

=== Listの使い方

データは二次元配列になる。単純な2次元配列を使うのも可能だが、今回はListを使ってみることにする。読み込んだ一次元のデータを複数並べてデータを作ること、データは一組で計算をする事が多いので、データ要素を明示的に取り出しやすいというメリットがある。単純に今まで使ったことがないから、練習を兼ねて使ってみようという面もある。

Listは、動的に要素を追加したり削除したりできる、動的リストである。したがって、任意の型の配列をそのままリストに追加したりすることもできる。

定義は、@<code>{var list = new List<int>();}で定義することができる。

簡単な使い方として、例を示す。

//listnum[listHowto][Listの使い方]{
//最初から値を設定
var list = new List<int>()
{
    1, 2, 3, 4, 5 // listには最初から1,2,3,4の4つの要素が含まれている
};
//値の追加・挿入   
list.add(5);        //リストの最後に5を追加する。
list.Insert(0,20);  //先頭(0番目)に20を追加

//配列で定義すれば、配列をそのままつけることもできる。
List<int[]> listB = new List<int[]>(); //int[]=配列を並べたListを定義
int dataArray[] = new int[];    //dataArrayは配列。
list.add(dataArray[]);          //末尾に配列を付加。

//要素の取り出し
int data[] = listB[2]; //3番目の要素(配列)を取り出す。
    //配列なので、受け入れ先は配列である必要がある。
//}

さて、@<list>{readCode}で読み込んだデータ列は、一旦仮配列addDataArray[k]に格納された。あとは、List.addで格納していけばよい。

そこで、生データ(電圧)を格納するListと、温度データ(変換後)を格納する2つのListを定義する。

ところで、ここで使うTemeratureListはグラフ描画の際にも利用するので、プロジェクト全体から参照できるようにしておく。
そのため、Form1のClass宣言のあとに置く。それ以外にも、全体から参照する何かが出てきたら、ここに置くようにすればよい。

//listnum[public][参照スコープを広げる]{
namespace FTIRanalyzer
{
    public partial class Form1 : Form
    {
        List<Double[]> rawData = new List<Double[]>();
        List<Double[]> temperatureData = new List<Double[]>();
    
	    public Form1()
        {
            //略
        }
        private void button1_Click(object sender, EventArgs e)
        {
            //略
	    }
    }
}
//}
この位置に置くことで、Form1の中の処理において、直ちに参照する事ができる。



=== 別Classから参照するには

なお、別ウィンドウ（別のClass）から参照する場合には、オブジェクト参照を行う必要がある。

Form1で定義したものをForm2内で呼び出す場合：

//listnum[Object][別クラスから参照するコード例]{
NG:パブリックなオブジェクトでないのでそもそもダメ。
Class Form1
    int A;
Class Form2
	int test = Form1.A; 
        //アクセスできない保護レベルになっています。。

NG:オブジェクト参照が必要というメッセージが出る。
Class Form1
	public int A;
Class Form2
	int test = Form1.A; 
        //静的でないオブジェクト、メソッド、またはプロパティForm1.Aでオブジェクト参照が必要です。

OK:静的オブジェクトにして参照する。
Class Form1
	public static int A;
Class Form2
	int test = Form1.A; //OK
//}

参照するオブジェクトを静的オブジェクトにしてしまう@<fn>{staticObject}と、参照できるようになる。なぜこれで解決するのかは全体的に謎である。が、そういうものであると一度把握していると、次から悩まなくてすむ。なお、新しいクラスを作って、そのオブジェクトに対して呼び出す方法もある@<fn>{obj}ようだが、これはさらによくわからなかった。もっと詳しい人あるいは詳しい本の説明を参照されたい。

//footnote[staticObject][http://shirakamisauto.hatenablog.com/entry/2015/06/15/181428]
//footnote[obj][http://www.atmarkit.co.jp/ait/articles/1806/06/news023.html#createobject 解法1;オブジェクトを作る]

== 計算をする

計算は、基本的には当たり前の式が使えるので、簡単である。
四則演算、たす、ひく、かける、わる、についてはいずれも基本的にはそのままである。
かっこも通常のままで、ほぼ通常の優先順位のままと考えれてよい。

ほかによく使う計算としては、以下の通りである。

Logの計算は、Math.Log(Double）を用いる。自然対数（e)に対する対数を返す。
常用対数を用いる場合は、Math.loh10(Double)を使えばよい。また、任意の底に対する対数を取るには、Math.Log(double a,double base)を用いる。負の値など、定義できない場合はNaNが出力されるので、データを確認するなどする必要がある。

三角関数はそのまま使える。ただし、引数はラジアンなので、Degree*2*Math.Pi()/360などとして、ラジアンに換算して与える必要がある。

絶対値を使う場合には、Absメソッドを使う。ルートはSqrtを用いる。その他にも、丸める関数など、基本的なものは標準的に存在するので、必要になったら例えば下記の公式リファレンス@<fn>{MathRef}を参照されたい。

//footnote[MathRef][https://msdn.microsoft.com/ja-jp/library/system.math_methods(v=vs.110).aspx]

むしろ計算を行う場合に注意しなければならないのは、型の問題である。C#では、原則異なる型の数値を直接計算することはできない。筆者は何度それで煮え湯を飲まされたことか。

まず整数型変数と浮動小数点型変数があることくらいは把握していると思う。数値型もたくさんあるが、よく使うものだけを示す。

 * byte 0~255.符号なし8bit整数。描画のカラーテーブルなどで使用
 * int -21億～＋21憶。符号付32bit整数。カウント等でも普通に使う。
 * float 32bit浮動小数点。Singleとも書く。有効数字7桁
 * double 64bit浮動小数点(倍精度)有効数字約15桁。

数値は型によって表示できる範囲が異なるので、型を混ぜた計算を行おうとすると、予期せぬ答えが出たり、エラーが出たりする。

以下に型混在での計算例を示す。

@<code>{int a = 6;　int b = 3;　int c = a / b;}

答えはなんだろう？c=2である。
では、


@<code>{int a = 6;　int b = 4;　int c = a / b;}

となるとどうなるか？
答えは、c=1が返る。切り捨てされるのである。
では、マイナスが含まれる場合はどうなるか。

@<code>{int a = -8;　int b = 3;　int c = a / b;}

答えは、c=-2である。Intは小数点切り捨てになるようなので、入力が整数同士であっても、それを使って演算する場合に予期せず小数が現れ、その結果がずれてしまうことがありうる。

@<code>{double a = -8;  int b = 3;　int c = a / b;}

とすると、
「エラー　型 'double' を 'int' に暗黙的に変換できません。明示的な変換が存在します。(cast が不足していないかどうかを確認してください)」というコンパイルエラーが出る。。これは、浮動小数点が含まれる数式を整数型に代入する操作を行っているため、数値のズレが生じるためである。広い数値型を狭い数値型に代入するとエラーになるのである。

したがって、例えば、型を混ぜた計算をすると、以下のようになる。
//listnum[code_kata][型混合の計算の例]{
double a = -8;  int b = 3;　double c = a / b;   　//OK
Single a = -8;  Single b = 3;　Single c = a / b;　//OK
Single a = -8;  Single b = 3;　Double c = a / b;　//OK
Double a = -8;  Double b = 3;　Double c = a / b;　//OK
Double a = -8;  Double b = 3;　Single c = a / b;　//NG（コンパイルエラー）
Double a = -8;　Int16 b = 3;　 int c = (int)(a / b);　//OKだが、整数に丸められる。
//}

このように、数値が表現可能な範囲は、倍精度浮動小数点（Double）>単精度浮動小数点（Single・Float) > 整数（int）と、広いからである。同様に、整数型の中でもInt(32bit)とShort(16bit)などで同じ現象が起こる。

このことは、プログラミングになれた人にとっては当たり前の内容であるが、初めての人には案外躓く内容である。実際この章の実装を書くにあたって実装している途中で型で苦しめられるのは何回もあったからである。また、筆者はVBから入った人なので、その差にも戸惑った。VBは型の制約が緩いためである。これを気持ち悪いという向きもあろうし、バグや意図しない挙動の原因となるという向きもあろう。

//listnum[kataVB][VBでは大丈夫な計算例]{
Double a = -8  Double b = 3　Single c = a / b　’OKSingle精度に丸められる。エラーは出ない。
//}

また、数値と文字列の変換も必要な時がある。例えば数値をラベルやテキストとして表示したいことは多々あろう。逆に入力した値を数値化するなどもある。そこで、ラベルへの数値の書き込み、読み取りを例に、数値→文字列、文字列→数値の変換例を示す。
　　　　
//listnum[code_tostring][文字列（String)と数値の変換]{
int c = 500;    //数値として定義
label1.Text = c.ToString();　　//数値を文字列に変換して、ラベルに表示

string t = "12345";　//文字列として定義
label1.Text=t;      //ラベルに表示
int b= Int32.Parse(t);  //数値に変換
label2.Text = b.ToString(); //数値を文字列に変換して、ラベルに表示
//}

== 画像を描画する

計算結果をグラフにするとか、二次元画像として表示するために、画像を表示する。グラフコンポーネントを使ってもよいが、本節では、ビットマップとして絵を作る方法での描画を行う。描画領域を確保し、それぞれの画素をデータに応じた色(グレースケールまたはカラー)に塗り分け、画像とするものである。

ソフトの実装は以下の通りであり。
最も基本的なものとして、全体を塗る場合を考える。デザイナー画面で適当な大きさのPictureBoxを置く。

//listnum[Code_Bitmap][ビットマップの宣言・黒塗り]{
Bitmap canvas = new Bitmap(pictureBox1.Width, pictureBox1.Height);
   	//新しいビットマップを定義する。大きさはPictureboxのサイズ。
Graphics g = Graphics.FromImage(canvas);
g.FillRectangle(Brushes.Black, 0, 0, 150, 100);　
    //塗る色は黒。四角形で塗る。開始点、終了点(幅、高さ)を指定する。
g.Dispose();　//Graphicを解放
pictureBox1.Image = canvas;　　イメージを書き込む
//}

Brushesで塗る色を設定できるので、様々な色を試してみるとよい。

次に、各ピクセルの色/輝度を指定して塗る方法である。

//listnum[Code_changeColor][塗る色を変える方法]{
Bitmap canvas = new Bitmap(pictureBox1.Width, pictureBox1.Height);
for (int x = 0; x < pictureBox1.Width; x++)
{
    for (int y =0; y<pictureBox1.Height;y++){
        Color gladc = Color.FromArgb(x,x,x);
        canvas.SetPixel(x,y,gladc);
    }
}
pictureBox1.Image = canvas;
//}

この例でRGBにx,x,xを入れているので、縦のグラデーションになる。y,y,yとすれば横グラデーションになり、x+yを入れれば斜めグラデーションになる。また、RGBでカラー設定ができるので任意の色を設定する事ができる。

//image[brushColor][位置により色を変えるテスト]

なお,FromArgbに設定可能な値は0～255であり、256以上、または負の値を指定すると例外が返ってきてしまう。既存データを利用する場合には、注意しなければならない。

そこで、例えば、

//listnum[errorcare][エラーのケア]{
if (x > 255){
    x = 255;
}
if (x < 0){
    x = 0;
}
//}

などとして、例外のケアができると、エラーの少ないコーディングができるし。これを事前に察知する事ができれば、ソフト実装のクオリティも上がるはずである。 こういう「想定外」を想定内にするテクニックが重要になってくるのはそうかもしれないが、訓練あるのみなのか？

さて、描画にあたっては、SetPixel等を用いて、配列の数値を順に並べることで二次元プロットを行うことができる。エクセルにも3Dグラフがあるが、カラー区切りが大雑把なので、256階調で直接プロットしたほうが精度の高いグラフを作ることができる。また、単純な折れ線グラフ、散布図では一列のみの表示、または重ね書きをするしかなく、理解しづらいグラフとなる。

そのほか、よく使うと思われる描画メソッドのコード例を示す。

==== 線を引く
画像中に線を引く場合、DrawLineが使える。

//listnum[DrawLine][Drawlineで線を引く]{
Graphics g = Graphics.FromImage(img);
Pen p = new Pen(Color.White, 1);
g.DrawLine(p, 64, 0, 64, 400);
pictureBox1.Image=img;
//}

==== 塗りつぶす
//listnum[FillRectangle][FillRectangleで四角を塗りつぶす]{
G.FillRectangle(Brushes.Black, 0, 0, img.Width, img.Height);
//}

黒で、imgの幅×高さの領域を塗りつぶす。@<img>{brushColor}の左上段はFillRectangleの例。
FillRectangleでは、開始点X、開始点Y、終了点X、終了点Yを指定する。

=== 描画部分の機能

また、生の数値に適当な倍率をかけてスケーリングすることで、適切な輝度の2次元画像を得ることができる。このような処理により2次元画像にしたときのパターン抽出、傾向把握の効果については先述したとおりである。

また、Redrawボタンをつけておくと、輝度を変えたときなどに描き直すことができて便利になる。

画像をクリックしたときにあると便利な機能は、

 * 現在位置の十字バーを表示する。
 * 時間とクリック地点の値を表示する。
 * X軸データとY軸データの折れ線グラフを表示する。であろう。

それを図に示す。

いずれも、実装は比較的簡単で、以下の手順による。

 1. クリック位置を取得する。
 2. 当該座標におけるデータを抽出、表示する。
 3. 十字バーを表示する。
 4. X軸データ、Y軸データをグラフ化する。

=== XY座標の読み取り

画像上でクリックしてXY位置を取得する。用途は、画像データの値を確認したり、断面グラフを出力したり、である。
画面クリック時のイベントとして、X座標、Y座標を取得するコードを書く。

//listnum[xyread][画像内座標の読み取り]{
private void pictureBox3_MouseDown(object sender, MouseEventArgs e)
{
    tbXYplace.Text = e.X+","  +e.Y+ ",";
        //e.XでX座標、e.YでY座標が取得可能。Int。
}
//}

これは単純に、PictureBox内のX座標Y座標を取得し、表示する例である。

このX、Yを例えば別のPictureBoxに渡してグラフを再描画するなどのコードを書けば良い。注意しなければならないのは、他の
関数に値を渡す場合、配列の外に出てしまったりすることがあるのである。

例えば、X座標の値を表示するコードを書いているのだが、もとのデータにはクリックしたX座標までデータがなく、インデックスが元データの境界外となってしまうようなエラーが発生しうる。

//listnum[Reference][境界外エラーが出やすい例]{
placeX = e.X;
lblDate.Text = measureDate[placeX].ToString();
//}
measureDataには20までしかデータがないのに、クリック位置が40である、といった例である。このようなとき、簡単にExceptionが出てしまう。

そこで、例えば、
//listnum[care_outofindex][境界外エラーの手当て例]{
if (e.X>=Datacount){
	placeX=Datacount-1; //データ数より多いところは指定できないようにする
}
else{
	placeX = e.X;
}
lblDate.Text = measureDate[placeX].ToString();
//}
として、エラーを発生させないようにする必要がある。

VisualStudioの中で実行している分にはエラーがわかりやすいが、通常の環境では、どんなエラーがでているのか分かりづらく、まして人に使ってもらう際には、こういうエラーはなくしたいものである。

== ファイル書き出し

せっかく解析したファイルなので、データを書き出し、次回効率よく解析したいというニーズがでて来るはずである。そこで、ファイルの書き出し方法について解説する。

=== 画像ファイルの書き出し

画像を書き出すにあたっては、スクリーンショットをとっても良いのだが、それでは画質が低下したり、周囲のウィンドウ部分が入り込んでしまうなど、解析、報告書等に使うのは難しい。当然ソフトの方で画像として保存することは可能である。

ツールバーやボタンから保存してもよいが、描画ウィンドウをダブルクリックしたら保存できる、という仕様を実装してみる。
デザインウィンドウから、保存したい画像が出る画面をクリックする。ここでは、pictureBox2に保存対象の絵が描かれるとする。

通常のプロパティ画面ではなく、イベント画面を選択するため、雷のようなマークをクリックする。@<img>{DoubleClickEvent}参照。すると、アクションというカラムがあり、そこにClick、DoubleClickというセルがあるので、DoubleClickのセルをダブルクリックする。すると、ダブルクリックしたときのイベントが追加される。「pictureBox2をダブルクリックしたとき」というそのままなイベント名がつけられているので、その中に画像をビットマップとして保存するコードを追記する。画像を保存するコードは@<list>{doubleclickevent}である。ビットマップの保存のコードは非常に簡単である。

//image[DoubleClickEvent][ダブルクリックイベントの指定方法]

//listnum[doubleclickevent][ダブルクリックイベント]{
private void pictureBox2_DoubleClick_1(object sender, EventArgs e)
{
    SaveFileDialog sfd = new SaveFileDialog();
	    //新しいSaveFileDialogを作る
    sfd.Filter = "bmp|*.bmp";
	    //フィルターはbmp
    sfd.RestoreDirectory = true;
	    //前回のフォルダを記憶
    if (sfd.ShowDialog() == DialogResult.OK)
	    //SaveFileDialogが開けたら
    {
	    img2.Save(sfd.FileName);
		//img2を保存する。
    }
}
//}


=== CSVファイルの書き出し

解析したデータを数値データとして書き出す方法についても記述する。
エクセルでのあと解析を行ったり、生データとして保存するなど、CSVファイルで書き出す必要はある。
書き出しのコードは以下の通り。
400列×元データ数分の行のCSVを生成する。合わせて、最上部に測定日を表示するコードである。csvなどを書きだすには、StreamWriterを使い、一ワードづつ記載する。

 * 1日目　2日め・・・
 * Data1　Data1
 * Data2　Data2
 * ・・・

というCSVファイルを作る。

//listnum[saveCSV][CSVを保存する]{
private void btnSave_Click(object sender, EventArgs e)
{
    SaveFileDialog sfd1 = new SaveFileDialog();
    sfd1.Filter = "csv|*.csv";
    sfd1.ShowDialog();

    if (sfd1.FileName != "")
    {
        FileStream fs = new FileStream(sfd1.FileName,FileMode.Create);
			//ファイルを「新規ファイル」で作る。
        StreamWriter sw = new StreamWriter(fs);
			//書き込み用ストリームライターを新規生成
        sw.Write(",");
			//区切り文字はカンマ
        for (int i = 0; i < filecount; i++)
        {
            sw.Write(measureDate[i].ToString());
			//測定日時を書き込む
            sw.Write(",");
			//カンマで区切り
        }
        sw.WriteLine();
			//ヘッダ行書き終わったの改行

        for (int j = 0; j < 400; j++)
			//縦は1データ400列。1行ごとに書いていくので、先に400カウント。
        {
            datanumber = j;
			    //測定回数を入れておく。
            sw.Write(datanumber.ToString());
			    //数値を文字列に変換して記入。
            sw.Write(",");
			    //カンマで区切り

            for (int i = 0; i < filecount; i++)
			    //データ列の数だけカウント
            {
                Double[] item = Data[i];
				    //listに格納した元データからデータを取ってくる
                sw.Write(item[j]);
				    //j行目を書き込む
                sw.Write(",");
            }
            sw.WriteLine();
				//1行書き終わったので、改行。
        }

        sw.Close();
			//書き込み用ストリームを閉じる
        fs.Close();
			//ファイルを閉じる
    }
}
//}


=== IfとSwitch文

選択肢が複数になる場合、Switch文@<fn>{swichRef}を使うと、コードがシンプルになる。

//listnum[swich][Switch文]{
switch (cmbboxSelectData.SelectedIndex)
	//引数は、コンボボックスのアイテムの番号。0スタート。
{
    case 0: //1個め(Index0)選択時             
        Console.Write("case0:1個め選択");
        break;
    case 1: //2個め（Index1)選択時
        Console.Write("case1:2個め選択");
        break;
}
//}

確かにifで複数条件を設定するよりも簡単に書ける気がする。2分岐ならIFでも良いが、3つ以上であればSwich文のほうが見通しが良い。

なお、注意事項は3点。

 * 引数に使えるのは整数型(int)または文字列(String)。

 条件判定に使えるのは、整数または文字列で、浮動小数点(floatやDouble)やBool等は使用できない。

 * break;を必ず入れる。
各条件の最後には、必ずbreak;を入れる必要がある。ここでループを抜ける。

 * 条件の終わりは、コロン(:)
C#では各文末をセミコロン(;)で閉じるが、ここはコロン(:)である。間違えないように。

//footnote[swichRef][https://udemy.benesse.co.jp/development/c-sharp-switch.html]

== まとめ
C#を前提に、簡単なプログラムを作りながら、要件定義から、いくつかプログラミングテクニックや画像化のメリットについて述べた。実施している内容はそれぞれそれほど難しくないはずなので、興味のある言語でやってみていただきたい。（なお、バグがある可能性はあるので、本記事の通りにやったからといって動くとは限らない）。