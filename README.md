# keyToMouse  
keyToMouse1.cpp use window message (foregroundwindowが変更されてしまう)   
keyToMouse2.cpp use timer(ちらつきあり)  
keyToMouse3.cpp use hook(recommendation)  
#  keyToMouse3.cppの注意  
システムhookを使用しているので異常終了などhookの後処理が十分にできなかった場合に気を付けてください
# keyToMouse3.cpp使い方
## アクティブにする方法  
ctrl+mでアクティブにできます。  
## アクティブになってから  
### カーソル移動
j=左  
l=右  
i=上  
k=下  
spaceを押すと早くなります。   
shiftを押すと遅くなります。 
逆になる可能性があります  
### マウスボタン  
s=左クリック  
f=右クリック  
d=ミドルボタンのクリック  
キーを押している間はボタンを押しっぱなしにしている状態と同じです。  
キーを離すとボタンを離したのと同じです。  
### スクロール  
e=上にスクロール  
m=下にスクロール  
g=左にスクロール  
h=右にスクロール  
次のリリースでGとHは逆に変更される可能性があります。(手の位置では左右正しいが手の動きで考えたときに違和感があるから)  
また、mとtの上下スクロールもwとnに変更される可能性があります(快適かどうか)  
## 終了
c=非アクティブにする(アクティブフラグがfalseになるだけなので再度アクティブにすることができます)  
q=終了(プロセスが終了するので再度起動しないとアクティブにすることはできません)  
