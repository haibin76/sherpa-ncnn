/**
 * Copyright (c)  2022  Xiaomi Corporation (authors: Fangjun Kuang)
 *
 * See LICENSE for clarification regarding multiple authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <cctype>  // std::tolower

#include "portaudio.h"  // NOLINT
#include "sherpa-ncnn/csrc/display.h"
#include "sherpa-ncnn/csrc/microphone.h"
#include "sherpa-ncnn/csrc/recognizer.h"

bool stop = false;

static int s_file_idx = 0;
static int s_file_num = 222;
static float *s_file_buffer = NULL;
static int s_sample_total = 0;
static int s_sample_num = 0;
static int s_sample_idx = 0;
static FILE *s_file_fp = NULL;
static const char *s_file_table[222] = {
    "D:/work/asr/stt_ali/2024-5-14/1009822953382006_1715657683200/270166038.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1009822953382006_1715657683200/155371212.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1009822953382006_1715657683200/309340642.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1009822953382006_1715657683200/335725653.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1009822953382006_1715657683200/715186461.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1009822953382006_1715657683200/842363928.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1523489263109663_1715658029745/206074196.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1523489263109663_1715658029745/482994572.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1523489263109663_1715658029745/891776888.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1523489263109663_1715658029745/990073801.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1585245779568436_1715657740314/207275182.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1585245779568436_1715657740314/364742613.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1585245779568436_1715657740314/520051565.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1585245779568436_1715657740314/556638789.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1585245779568436_1715657740314/877378473.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1699044034602778_1715657944196/103779756.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1699044034602778_1715657944196/416675780.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1699044034602778_1715657944196/578328779.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1699044034602778_1715657944196/599814570.pcm",
    "D:/work/asr/stt_ali/2024-5-14/1699044034602778_1715657944196/817161981.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2003141066031278_1715657890760/172947725.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2003141066031278_1715657890760/459899058.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2003141066031278_1715657890760/916711987.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2003141066031278_1715657890760/925001358.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2241255284057669_1715657793697/240500998.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2241255284057669_1715657793697/409621743.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2241255284057669_1715657793697/445034908.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2241255284057669_1715657793697/982915591.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2369643897088349_1715657763567/325861977.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2369643897088349_1715657763567/337361515.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2369643897088349_1715657763567/342797625.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2369643897088349_1715657763567/490620117.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2369643897088349_1715657763567/896029180.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2369643897088349_1715657763567/981880954.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2489437662874952_1715657542383/414021132.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2489437662874952_1715657542383/624448655.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2489437662874952_1715657542383/705244368.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2489437662874952_1715657542383/803866227.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2489437662874952_1715657542383/914950744.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2568603525497277_1715657550513/139332172.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2568603525497277_1715657550513/213773466.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2568603525497277_1715657550513/238377957.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2568603525497277_1715657550513/255072785.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2568603525497277_1715657550513/791198471.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2596492131341372_1715657524260/201814065.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2596492131341372_1715657524260/466126585.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2596492131341372_1715657524260/506723476.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2596492131341372_1715657524260/609963109.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2596492131341372_1715657524260/686759419.pcm",
    "D:/work/asr/stt_ali/2024-5-14/2596492131341372_1715657524260/992875512.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3067022572745844_1715657917760/187878232.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3067022572745844_1715657917760/277798542.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3067022572745844_1715657917760/589208395.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3067022572745844_1715657917760/663150252.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3067022572745844_1715657917760/854089764.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3067022572745844_1715657917760/950183110.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3094349558149313_1715657638472/114208128.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3094349558149313_1715657638472/461379512.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3094349558149313_1715657638472/606877903.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3094349558149313_1715657638472/786013363.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3094349558149313_1715657638472/899826441.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3176042390388804_1715657801201/175313619.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3176042390388804_1715657801201/247129735.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3176042390388804_1715657801201/451392142.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3176042390388804_1715657801201/594591652.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3176042390388804_1715657801201/816454625.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3176042390388804_1715657801201/817640970.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3231559924749779_1715657630262/473194315.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3231559924749779_1715657630262/729295926.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3231559924749779_1715657630262/748100688.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3231559924749779_1715657630262/828362432.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3231559924749779_1715657630262/843627309.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3231559924749779_1715657630262/877401929.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3412399392322928_1715657742682/160140123.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3412399392322928_1715657742682/202395883.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3412399392322928_1715657742682/208687026.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3412399392322928_1715657742682/699423058.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3412399392322928_1715657742682/967355523.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3722512429832816_1715657680333/102939613.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3722512429832816_1715657680333/146824396.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3722512429832816_1715657680333/757208246.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3722512429832816_1715657680333/792012341.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3842731749660628_1715658001392/188402001.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3842731749660628_1715658001392/431025710.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3842731749660628_1715658001392/581408133.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3842731749660628_1715658001392/869420974.pcm",
    "D:/work/asr/stt_ali/2024-5-14/3842731749660628_1715658001392/941624538.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4031454118169720_1715657965485/108126944.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4031454118169720_1715657965485/269329193.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4031454118169720_1715657965485/269329879.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4031454118169720_1715657965485/269534330.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4031454118169720_1715657965485/407156523.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4031454118169720_1715657965485/483588740.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4183446993406385_1715657999712/456996149.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4183446993406385_1715657999712/662877207.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4183446993406385_1715657999712/721368351.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4183446993406385_1715657999712/774999079.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4183446993406385_1715657999712/879177997.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/103808262.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/319162041.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/326258691.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/798900622.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/812688295.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/894497091.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4370773645287973_1715657531983/899421398.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4666190050524236_1715657865910/356921944.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4666190050524236_1715657865910/533690101.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4666190050524236_1715657865910/714615514.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4666190050524236_1715657865910/964415407.pcm",
    "D:/work/asr/stt_ali/2024-5-14/4666190050524236_1715657865910/986687592.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/101358509.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/173263850.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/193081649.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/238407684.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/264960410.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/473041548.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/607655468.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/658325772.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/715839898.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/741006579.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5402610694529036_1715657667094/975759099.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5502634638025508_1715657982779/298781726.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5502634638025508_1715657982779/313842566.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5502634638025508_1715657982779/317706250.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5502634638025508_1715657982779/820732055.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5502634638025508_1715657982779/876884428.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5634238512998329_1715657610817/237107509.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5634238512998329_1715657610817/246099840.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5634238512998329_1715657610817/472809577.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5634238512998329_1715657610817/516750381.pcm",
    "D:/work/asr/stt_ali/2024-5-14/5634238512998329_1715657610817/521518181.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6404502189075040_1715657726161/203443689.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6404502189075040_1715657726161/411355251.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6404502189075040_1715657726161/596971390.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6404502189075040_1715657726161/684481803.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6404502189075040_1715657726161/859834711.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6404502189075040_1715657726161/941131239.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6522222457566153_1715657551255/237804061.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6522222457566153_1715657551255/287739045.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6522222457566153_1715657551255/526731825.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6522222457566153_1715657551255/907031587.pcm",
    "D:/work/asr/stt_ali/2024-5-14/6522222457566153_1715657551255/936261098.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7006010060874977_1715657828403/568373664.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7006010060874977_1715657828403/663841984.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7006010060874977_1715657828403/716180653.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7006010060874977_1715657828403/969044141.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/330728387.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/359599604.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/535230128.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/596421897.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/719762932.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/724586909.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7047981979819671_1715657833724/933294742.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7085527429019704_1715657734653/157279005.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7085527429019704_1715657734653/595102166.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7085527429019704_1715657734653/651897960.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7085527429019704_1715657734653/660287399.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7202632972114538_1715657660093/149414008.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7202632972114538_1715657660093/265819107.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7202632972114538_1715657660093/546167935.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7202632972114538_1715657660093/939660369.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7369472596425680_1715657708706/216662199.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7369472596425680_1715657708706/423670360.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7369472596425680_1715657708706/480725532.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7369472596425680_1715657708706/496333759.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7369472596425680_1715657708706/509032395.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7369472596425680_1715657708706/651039605.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7575760596476908_1715657628736/149059791.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7575760596476908_1715657628736/180151684.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7575760596476908_1715657628736/193412677.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7575760596476908_1715657628736/761921632.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7575760596476908_1715657628736/844996348.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7844500177520834_1715657949410/141187014.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7844500177520834_1715657949410/431539236.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7844500177520834_1715657949410/485446581.pcm",
    "D:/work/asr/stt_ali/2024-5-14/7844500177520834_1715657949410/918124862.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8228649577260428_1715658022597/349398075.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8228649577260428_1715658022597/612219914.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8228649577260428_1715658022597/691222633.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8228649577260428_1715658022597/783444442.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8228649577260428_1715658022597/956312247.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8229864636961960_1715657924558/287160797.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8229864636961960_1715657924558/429388297.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8229864636961960_1715657924558/657283619.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8269180962915559_1715657770600/384593368.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8269180962915559_1715657770600/400586594.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8269180962915559_1715657770600/886079284.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8269180962915559_1715657770600/889474271.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8446990373008949_1715657762542/515557307.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8446990373008949_1715657762542/642734842.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8446990373008949_1715657762542/774647173.pcm",
    "D:/work/asr/stt_ali/2024-5-14/8446990373008949_1715657762542/966672498.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/371211377.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/391352432.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/404108111.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/580593809.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/667257715.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/804274609.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9041740349025757_1715657477396/932349632.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9248803496481643_1715657592574/160664628.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9248803496481643_1715657592574/346242583.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9248803496481643_1715657592574/371536409.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9248803496481643_1715657592574/834896036.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9248803496481643_1715657592574/901849345.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9414126796572200_1715657764801/251379227.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9414126796572200_1715657764801/288564334.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9414126796572200_1715657764801/454646970.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9414126796572200_1715657764801/531143989.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9414126796572200_1715657764801/715924084.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9414126796572200_1715657764801/822584023.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/285081306.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/331690817.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/711031470.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/734169652.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/779300635.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/798429675.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9481179489597464_1715657486176/841955878.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9670865817429140_1715658039064/156735265.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9670865817429140_1715658039064/719026211.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9670865817429140_1715658039064/813761674.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9670865817429140_1715658039064/843473572.pcm",
    "D:/work/asr/stt_ali/2024-5-14/9670865817429140_1715658039064/942853603.pcm"
};

static int pcm_file_convert(int sample_num)
{
    //对文件buffer进行重新采样，并且结果还是要保存到buffer中
    //48k * 2 => 16K * 1, 3个sample平均成一个sample
    short *short_buffer = (short*) s_file_buffer;
    float *float_buffer = (float*) s_file_buffer;
    int sample_num_16k = sample_num / 3;
    for (int i = 0; i < sample_num_16k; i++) {
        int val = (short_buffer[i * 6 + 0] + short_buffer[i * 6 + 1] + short_buffer[i * 6 + 2] + short_buffer[i * 6 + 3] + short_buffer[i * 6 + 4] + short_buffer[i * 6 + 5]) / 6;
        int z = val + 1;
        float_buffer[i] = (float)(val * 1.0 / 32768);
    }

    //增加300ms的静音
    for (int i = 0; i < 300 * 16; i++) {
        float_buffer[sample_num_16k + i] = 0.0;
    }

    return sample_num_16k + 300 * 16;
}

static bool read_pcm_frame(float *input_buffer, int sample_num)
{
    // 0 判断文件是否为空，如果是空，就打开下一个文件
    if (s_file_fp == NULL) {
        // 下一个文件是否为空，如果是空，则直接给与跳出
        if (s_file_idx >= s_file_num) {
            printf("finish reading the file\n");
            return false;
        }
        // 打开当前的pcm文件，并且要一次性的把数据读出来
        s_file_fp = fopen(s_file_table[s_file_idx], "rb");
        //s_file_fp = fopen("D:/work/asr/stt_ali/2024-5-14/8446990373008949_1715657762542/774647173.pcm", "r");
        if (!s_file_fp) {
            printf("read the file:%s, error\n", s_file_table[s_file_idx]);
            return false;
        }
        printf("\nopened the pcm file:%d, %s, total_num:%d\n", s_file_idx, s_file_table[s_file_idx], s_sample_total);
        fseek(s_file_fp, 0, SEEK_END);
        int file_length = (int)ftell(s_file_fp);
        fseek(s_file_fp, 0, SEEK_SET);
        int buffer_length = file_length + (16 * 300) * 4;  // 加上300ms的静音
        if (s_file_buffer) {
            free(s_file_buffer);
            s_file_buffer = NULL;
        }
        s_file_buffer = (float *)malloc(buffer_length);
        memset(s_file_buffer, 0, buffer_length);
        fread(s_file_buffer, 1, buffer_length, s_file_fp);
        s_sample_num = pcm_file_convert(file_length / (2 * sizeof(short)));
        s_sample_idx = 0;
    }

    // #1 读取一帧数据，将数据放入到input_buffer中
    if (sample_num < (s_sample_num - s_sample_idx)) {
        memcpy(input_buffer, &s_file_buffer[s_sample_idx], sample_num * sizeof(float));
        s_sample_idx += sample_num;
    } else {
        int sample_part1 = s_sample_num - s_sample_idx;
        memcpy(input_buffer, &s_file_buffer[s_sample_idx], sample_part1 * sizeof(float));
        int sample_part2 = sample_num - sample_part1;
        memset(&input_buffer[sample_part1], 0, sample_part2 * sizeof(float));
        s_sample_idx += sample_num;
        s_sample_total += s_sample_num;
        fclose(s_file_fp);
        s_file_idx++;
        s_file_fp = NULL;
        s_sample_idx = 0;
    }

    return true;
}

static int32_t RecordCallback(const void *input_buffer,
                              void * /*output_buffer*/,
                              unsigned long frames_per_buffer,  // NOLINT
                              const PaStreamCallbackTimeInfo * /*time_info*/,
                              PaStreamCallbackFlags /*status_flags*/,
                              void *user_data)
{
    static FILE *whb_fp = NULL;

    auto s = reinterpret_cast<sherpa_ncnn::Stream *>(user_data);
#if 0
    if (whb_fp == NULL)
        whb_fp = fopen("D:/work/asr/stt_ali/python/270166038.pcm", "rb");
    if (whb_fp) {
      int offset = ftell(whb_fp);
        fread((void *)input_buffer, 1, frames_per_buffer * 4, whb_fp);
    }
    if (feof(whb_fp)) {
        fseek(whb_fp, 0, SEEK_SET);
    }
#endif

    read_pcm_frame((float*)input_buffer, frames_per_buffer);

    s->AcceptWaveform(16000, reinterpret_cast<const float *>(input_buffer), frames_per_buffer);

    return stop ? paComplete : paContinue;
}

static void Handler(int32_t sig)
{
    stop = true;
    fprintf(stderr, "\nCaught Ctrl + C. Exiting...\n");
};

int32_t main(int32_t argc, char *argv[])
{
  if (argc < 8 || argc > 10) {
    const char *usage = R"usage(
Usage:
  ./bin/sherpa-ncnn-microphone \
    /path/to/tokens.txt \
    /path/to/encoder.ncnn.param \
    /path/to/encoder.ncnn.bin \
    /path/to/decoder.ncnn.param \
    /path/to/decoder.ncnn.bin \
    /path/to/joiner.ncnn.param \
    /path/to/joiner.ncnn.bin \
    [num_threads] [decode_method, can be greedy_search/modified_beam_search] [hotwords_file] [hotwords_score]

Please refer to
https://k2-fsa.github.io/sherpa/ncnn/pretrained_models/index.html
for a list of pre-trained models to download.
)usage";
    fprintf(stderr, "%s\n", usage);
    fprintf(stderr, "argc, %d\n", argc);

    return 0;
  }
  signal(SIGINT, Handler);

  sherpa_ncnn::RecognizerConfig config;
  config.model_config.tokens = argv[1];
  config.model_config.encoder_param = argv[2];
  config.model_config.encoder_bin = argv[3];
  config.model_config.decoder_param = argv[4];
  config.model_config.decoder_bin = argv[5];
  config.model_config.joiner_param = argv[6];
  config.model_config.joiner_bin = argv[7];
  int32_t num_threads = 4;
  if (argc >= 9 && atoi(argv[8]) > 0) {
    num_threads = atoi(argv[8]);
  }
  config.model_config.encoder_opt.num_threads = num_threads;
  config.model_config.decoder_opt.num_threads = num_threads;
  config.model_config.joiner_opt.num_threads = num_threads;

  const float expected_sampling_rate = 16000;
  if (argc == 10) {
    std::string method = argv[9];
    if (method == "greedy_search" || method == "modified_beam_search") {
      config.decoder_config.method = method;
    }
  }

  if (argc >= 11) {
    config.hotwords_file = argv[10];
  }

  if (argc == 12) {
    config.hotwords_score = atof(argv[11]);
  }

  config.enable_endpoint = true;

  config.endpoint_config.rule1.min_trailing_silence = 2.4;
  config.endpoint_config.rule2.min_trailing_silence = 1.2;
  config.endpoint_config.rule3.min_utterance_length = 300;

  config.feat_config.sampling_rate = expected_sampling_rate;
  config.feat_config.feature_dim = 80;

  fprintf(stderr, "%s\n", config.ToString().c_str());

  sherpa_ncnn::Recognizer recognizer(config);
  auto s = recognizer.CreateStream();

  sherpa_ncnn::Microphone mic;

  PaDeviceIndex num_devices = Pa_GetDeviceCount();
  fprintf(stderr, "Num devices: %d\n", num_devices);

  PaStreamParameters param;

  param.device = Pa_GetDefaultInputDevice();
  if (param.device == paNoDevice) {
    fprintf(stderr, "No default input device found\n");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Use default device: %d\n", param.device);

  const PaDeviceInfo *info = Pa_GetDeviceInfo(param.device);
  fprintf(stderr, "  Name: %s\n", info->name);
  fprintf(stderr, "  Max input channels: %d\n", info->maxInputChannels);

  param.channelCount = 1;
  param.sampleFormat = paFloat32;

  param.suggestedLatency = info->defaultLowInputLatency;
  param.hostApiSpecificStreamInfo = nullptr;
  const float sample_rate = 16000;

  PaStream *stream;
  PaError err =
      Pa_OpenStream(&stream, &param, nullptr, /* &outputParameters, */
                    sample_rate,
                    0,          // frames per buffer
                    paClipOff,  // we won't output out of range samples
                                // so don't bother clipping them
                    RecordCallback, s.get());
  if (err != paNoError) {
    fprintf(stderr, "portaudio error: %s\n", Pa_GetErrorText(err));
    exit(EXIT_FAILURE);
  }

  err = Pa_StartStream(stream);
  fprintf(stderr, "Started\n");

  if (err != paNoError) {
    fprintf(stderr, "portaudio error: %s\n", Pa_GetErrorText(err));
    exit(EXIT_FAILURE);
  }

  std::string last_text;
  int32_t segment_index = 0;
  sherpa_ncnn::Display display;
  system("chcp 65001");
  while (!stop) {
    while (recognizer.IsReady(s.get())) {
      recognizer.DecodeStream(s.get());
    }

    bool is_endpoint = recognizer.IsEndpoint(s.get());

    if (is_endpoint) {
      s->Finalize();
    }
    auto text = recognizer.GetResult(s.get()).text;
    auto time_sample = recognizer.GetResult(s.get()).timestamps;
    if (!text.empty() && last_text != text) {
      last_text = text;

      std::transform(text.begin(), text.end(), text.begin(),
                     [](auto c) { return std::tolower(c); });
      char temp[2048];
      sprintf(temp, "whb %s,time:%d--%d", text.c_str(), (int)(time_sample[0]*1000), (int)(time_sample[time_sample.size()-1] * 1000));
      std::string text2 = temp;
      display.Print(segment_index, text2);
    }

    if (is_endpoint) {
      if (!text.empty()) {
        ++segment_index;
      }

      recognizer.Reset(s.get());
    }

    Pa_Sleep(20);  // sleep for 20ms
  }

  err = Pa_CloseStream(stream);
  if (err != paNoError) {
    fprintf(stderr, "portaudio error: %s\n", Pa_GetErrorText(err));
    exit(EXIT_FAILURE);
  }

  return 0;
}
