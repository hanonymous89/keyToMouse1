# keyToMouse  
keyToMouse1.cpp use window message (foregroundwindowが変更されてしまう)   
keyToMouse2.cpp use timer(ちらつきあり)  
keyToMouse3.cpp use hook(recommendation)  
#  keyToMouse3.cppの注意  
システムhookを使用しているのでブルースクリーンが出る場合があります。（異常終了などhookの後処理が十分にできなかった場合）
