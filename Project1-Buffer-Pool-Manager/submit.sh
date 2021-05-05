mkdir -p src/include/buffer/
mkdir -p src/buffer/

cp ~/private-bustub/src/include/buffer/lru_replacer.h src/include/buffer/lru_replacer.h
cp ~/private-bustub/src/buffer/lru_replacer.cpp src/buffer/lru_replacer.cpp
cp ~/private-bustub/src/include/buffer/buffer_pool_manager.h src/include/buffer/buffer_pool_manager.h
cp ~/private-bustub/src/buffer/buffer_pool_manager.cpp src/buffer/buffer_pool_manager.cpp

zip -r submission.zip src/