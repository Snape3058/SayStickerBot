# TG_SayStickerBot

Say Sticker.

## Depend

```
sudo apt-get install g++ make binutils cmake
sudo apt-get install libssl-dev libboost-system-dev zlib1g-dev libcurl4-gnutls-dev
sudo apt-get install libcairo2-dev libpango1.0-dev libopencv-dev librsvg2-dev libwebp-dev
```

```
brew install openssl zlib cmake boost@1.59 pango
```

tgbot-cpp:
git clone https://github.com/YJBeetle/tgbot-cpp.git
git@github.com:YJBeetle/tgbot-cpp.git

ArtRobot: 
https://gitlab.com/project-null-plus-1/art_robot

## Build&&Test

    mkdir build && cd build && cmake ..
    make && TOKEN=<TOKEN> ./TG_ThrowItBot

## Run

    TOKEN=<TOKEN> ./TG_ThrowItBot