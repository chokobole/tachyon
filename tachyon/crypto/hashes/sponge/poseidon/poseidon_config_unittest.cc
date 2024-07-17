// Copyright 2022 arkworks contributors
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.arkworks and the LICENCE-APACHE.arkworks
// file.

// Copyright 2020-2022 The Electric Coin Company
// Copyright 2022 The Halo2 developers
// Use of this source code is governed by a MIT/Apache-2.0 style license that
// can be found in the LICENSE-MIT.halo2 and the LICENCE-APACHE.halo2
// file.

#include "tachyon/crypto/hashes/sponge/poseidon/poseidon_config.h"

#include "gtest/gtest.h"

#include "tachyon/math/elliptic_curves/bls12/bls12_381/fr.h"
#include "tachyon/math/elliptic_curves/bn/bn254/fr.h"

namespace tachyon::crypto {

TEST(PoseidonConfigTest, CreateDefault) {
  using F = math::bls12_381::Fr;
  F::Init();

  // clang-format off
  struct {
    size_t rate;
    bool optimized_for_weights;
    const char* ark00_str;
    const char* mds00_str;
    const char* pre_sparse_mds11_str;
    const char* sparse_matrices0_row0_str;
    const char* sparse_matrices0_col_hat0_str;
  } tests[] = {
    {2, false, "27117311055620256798560880810000042840428971800021819916023577129547249660720", "26017457457808754696901916760153646963713419596921330311675236858336250747575", "31572861855011475512382606616921906104344750972039951664002591285536508929230", "26017457457808754696901916760153646963713419596921330311675236858336250747575", "45002031655667752692013421831488230391427366420163524473680328253726687081265"},
    {3, false, "11865901593870436687704696210307853465124332568266803587887584059192277437537", "18791275321793747281053101601584820964683215017313972132092847596434094368732", "2108106763215922983756385546696463973291838994933596182422034944160876157649", "18791275321793747281053101601584820964683215017313972132092847596434094368732", "49345719521328172034838277331157113214823707529672007560154520504879206156031"},
    {4, false, "41775194144383840477168997387904574072980173775424253289429546852163474914621", "42906651709148432559075674119637355642263148226238482628104108168707874713729", "15437196313207028184595027310954691524950441166982499707812221868605459955411", "42906651709148432559075674119637355642263148226238482628104108168707874713729", "10181779415406284187381889070013202931247080432698561721955388732806458190395"},
    {5, false, "24877380261526996562448766783081897666376381975344509826094208368479247894723", "30022080821787948421423927053079656488514459012053372877891553084525866347732", "41282462701257075947013006155391485833105491235974228070643662400214881691179", "30022080821787948421423927053079656488514459012053372877891553084525866347732", "25425797330383986939975846820290517346860157674975997228782902665559946889085"},
    {6, false, "37928506567864057383105673253383925733025682403141583234734361541053005808936", "49124738641420159156404016903087065194698370461819821829905285681776084204443", "35800598945207625118890817688177275491353678673920006135125132915429192475164", "49124738641420159156404016903087065194698370461819821829905285681776084204443", "34625021416715173102665029268844443939380746795844108023422982627959005796115"},
    {7, false, "37848764121158464546907147011864524711588624175161409526679215525602690343051", "28113878661515342855868752866874334649815072505130059513989633785080391114646", "44491938904618613847994407045095973656969196969278246083112427805331450699170", "28113878661515342855868752866874334649815072505130059513989633785080391114646", "8926028492755867965967982271216543923231923400322693460465359777201449661067"},
    {8, false, "51456871630395278065627483917901523970718884366549119139144234240744684354360", "12929023787467701044434927689422385731071756681420195282613396560814280256210", "20311132450471438679777045368739749846430256075810793822492797240824631712305", "12929023787467701044434927689422385731071756681420195282613396560814280256210", "6933660636939353989684835502385885069457790959019942691610270765723791920275"},
    {2, true, "25126470399169474618535500283750950727260324358529540538588217772729895991183", "46350838805835525240431215868760423854112287760212339623795708191499274188615", "7158883025140081155494225743701858329187816274956472049209318749748286385675", "46350838805835525240431215868760423854112287760212339623795708191499274188615", "41670883512945275681742223625320813741394911884800875951841966885702720898119"},
    {3, true, "16345358380711600255519479157621098002794924491287389755192263320486827897573", "37432344439659887296708509941462699942272362339508052702346957525719991245918", "7958519462256252092814822553431332423682377900432236000051194247868170655859", "37432344439659887296708509941462699942272362339508052702346957525719991245918", "12274414826144340225302116246402906004290373108743222102581266917149646287343"},
    {4, true, "2997721997773001075802235431463112417440167809433966871891875582435098138600", "43959024692079347032841256941012668338943730711936867712802582656046301966186", "10413021892166711101546889546835760618201922853953446025508630969839060192488", "43959024692079347032841256941012668338943730711936867712802582656046301966186", "29530843475896489817079500621071992096288025894390099966285545833216805237706"},
    {5, true, "28142027771717376151411984909531650866105717069245696861966432993496676054077", "13157425078305676755394500322568002504776463228389342308130514165393397413991", "8300898524308309707541911161568970361437736916234574268744441252694987147170", "13157425078305676755394500322568002504776463228389342308130514165393397413991", "3240588120537050727514544172116345717623701774729251760980304699558444803744"},
    {6, true, "7417004907071346600696060525974582183666365156576759507353305331252133694222", "51393878771453405560681338747290999206747890655420330824736778052231938173954", "24222275870165113916436461163966424792382710354127099068687251172600576113531", "51393878771453405560681338747290999206747890655420330824736778052231938173954", "43948540160518214250861457381805606836318794030909098562142053171391717738303"},
    {7, true, "47093173418416013663709314805327945458844779999893881721688570889452680883650", "51455917624412053400160569105425532358410121118308957353565646758865245830775", "22905060369990498146159659222872936119675742537315187577396733984558281850317", "51455917624412053400160569105425532358410121118308957353565646758865245830775", "11987872510832066865294358830496364591392340836809357251883620152705994277910"},
    {8, true, "16478680729975035007348178961232525927424769683353433314299437589237598655079", "39160448583049384229582837387246752222769278402304070376350288593586064961857", "8538302158391180252284129046560291629920731949880152312949251278235616956158", "39160448583049384229582837387246752222769278402304070376350288593586064961857", "6216301443733598962788486379773077523144809558895509342363760374574669902279"},
  };
  // clang-format on

  for (const auto& test : tests) {
    PoseidonConfig<F> config =
        PoseidonConfig<F>::CreateDefault(test.rate, test.optimized_for_weights);
    ASSERT_TRUE(config.IsValid());
    EXPECT_EQ(config.ark(0, 0), *F::FromDecString(test.ark00_str));
    EXPECT_EQ(config.mds(0, 0), *F::FromDecString(test.mds00_str));
    EXPECT_EQ(config.pre_sparse_mds(1, 1),
              *F::FromDecString(test.pre_sparse_mds11_str));
    EXPECT_EQ(config.sparse_mds_matrices[0].row()[0],
              *F::FromDecString(test.sparse_matrices0_row0_str));
    EXPECT_EQ(config.sparse_mds_matrices[0].col_hat()[0],
              *F::FromDecString(test.sparse_matrices0_col_hat0_str));
  }
}

TEST(PoseidonConfigTest, CreateCustom) {
  using F = math::bn254::Fr;
  F::Init();

  // clang-format off
  struct {
    size_t rate;
    uint64_t alpha;
    size_t full_rounds;
    size_t partial_rounds;
    size_t skip_matrices;
    const char* ark00_str;
    const char* mds00_str;
    const char* pre_sparse_mds11_str;
    const char* sparse_matrices0_row0_str;
    const char* sparse_matrices0_col_hat0_str;
  } tests[] = {
      {2, 5, 8, 57, 0, "6745197990210204598374042828761989596302876299545964402857411729872131034734", "7511745149465107256748700652201246547602992235352608707588321460060273774987",  "20498480049173041451757161739353136932402063966867101132544382489060457121690", "7511745149465107256748700652201246547602992235352608707588321460060273774987", "8364259238812534287689210722577399963878179320345509803468849104367466297989"},
      {3, 5, 8, 31, 0, "13403165100170528731188983416904733323082063763924490907636477756087868446385", "4843064272860702558353681805605581092414485968533095609154162537440763859608", "18904002742018845139579665761306904915446670558152364574366010338917470570969", "4843064272860702558353681805605581092414485968533095609154162537440763859608", "17098920592029650201655138442655581967768635903769554042626458334412341558169"},
      {4, 5, 8, 57, 0, "19113776568595304904649638772185171763805916665512167111307269381068492077003", "9390358363320792447057897590391227342305356869000968376798315031785873376651", "10430399957950918663081154404041014339007209286885623491793224536133959093342", "9390358363320792447057897590391227342305356869000968376798315031785873376651", "4258476155330118372762506614067358362596124649116737498715627157925163661"},
      {5, 5, 4, 31, 0, "12911143707477888215006316149665736087938931277132002862716953775656297280505", "10942845101262402626166273431356787436787991939175819278065571096963239049995", "5243571363387738625655552142589814423245410823902064800321501427822383158829", "10942845101262402626166273431356787436787991939175819278065571096963239049995", "10233199471798935861979427851690416803067121439733943143275749690073539672323"},
      {6, 5, 2, 12, 0, "6891333877554147985347986116215540686446242805416768740851383136400218469817", "9837954178279989965317992196165220609182866932765981962230951853077616895744", "2342265246701838568043436561372198242203879164391121705534106671605750623757", "9837954178279989965317992196165220609182866932765981962230951853077616895744", "9504868060103463837598804825403938487759722464496492662464197532903433072082"},
      {7, 5, 6, 57, 0, "3674224614566270991667957870461358488780972465005715233452288530417333318480", "20096031343166107597256293287487285680099080393005092613845214101013675429510", "10312826920506585789288853057573225788967547772875440745847746432386053340298", "20096031343166107597256293287487285680099080393005092613845214101013675429510", "21032704002095809084962846202624190836816781835379410962327180744509481109422"},
      {8, 5, 8, 63, 0, "14715728137766105031387583973733149375806784983272780095398485311648630967927", "708458300293891745856425423607721463509413916954480913172999113933455141974", "1739241146470184391733348241765630726223090738057028604336245542891367839190", "708458300293891745856425423607721463509413916954480913172999113933455141974", "9912718281831214028971669318551955931380391283853329273384463785744511549623"},
  };
  // clang-format on

  for (const auto& test : tests) {
    PoseidonConfig<F> config = PoseidonConfig<F>::CreateCustom(
        test.rate, test.alpha, test.full_rounds, test.partial_rounds,
        test.skip_matrices);
    ASSERT_TRUE(config.IsValid());
    EXPECT_EQ(config.ark(0, 0), *F::FromDecString(test.ark00_str));
    EXPECT_EQ(config.mds(0, 0), *F::FromDecString(test.mds00_str));
    EXPECT_EQ(config.pre_sparse_mds(1, 1),
              *F::FromDecString(test.pre_sparse_mds11_str));
    EXPECT_EQ(config.sparse_mds_matrices[0].row()[0],
              *F::FromDecString(test.sparse_matrices0_row0_str));
    EXPECT_EQ(config.sparse_mds_matrices[0].col_hat()[0],
              *F::FromDecString(test.sparse_matrices0_col_hat0_str));
  }
}

}  // namespace tachyon::crypto
