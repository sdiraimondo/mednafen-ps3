ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET		:=	libnestopia
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src

CPPFLAGS	+=	-DNST_NO_ZLIB -DNST_NO_SCALEX -DNST_NO_HQ2X -DNST_NO_2XSAI -DNST_PRAGMA_ONCE

CXXFILES	+=	src/fileio.cpp  src/input.cpp  src/mednafen.cpp  src/settings.cpp

CXXFILES	+=	src/core/NstApu.cpp              src/core/NstCpu.cpp            src/core/NstPatcher.cpp      src/core/NstSoundRenderer.cpp     src/core/NstVideoFilterNtsc.cpp						\
				src/core/NstAssert.cpp           src/core/NstCrc32.cpp          src/core/NstPatcherIps.cpp   src/core/NstState.cpp             src/core/NstVideoFilterScaleX.cpp					\
				src/core/NstCartridge.cpp        src/core/NstFds.cpp            src/core/NstPatcherUps.cpp   src/core/NstStream.cpp            src/core/NstVideoRenderer.cpp						\
				src/core/NstCartridgeInes.cpp    src/core/NstFile.cpp           src/core/NstPins.cpp         src/core/NstTracker.cpp           src/core/NstVideoScreen.cpp							\
				src/core/NstCartridgeRomset.cpp  src/core/NstImage.cpp          src/core/NstPpu.cpp          src/core/NstTrackerMovie.cpp      src/core/NstXml.cpp									\
				src/core/NstCartridgeUnif.cpp    src/core/NstImageDatabase.cpp  src/core/NstProperties.cpp   src/core/NstTrackerRewinder.cpp   src/core/NstZlib.cpp									\
				src/core/NstCheats.cpp           src/core/NstLog.cpp            src/core/NstRam.cpp          src/core/NstVector.cpp																	\
				src/core/NstChecksum.cpp         src/core/NstMachine.cpp        src/core/NstSha1.cpp         src/core/NstVideoFilter2xSaI.cpp														\
				src/core/NstChips.cpp            src/core/NstMemory.cpp         src/core/NstSoundPcm.cpp     src/core/NstVideoFilterHqX.cpp															\
				src/core/NstCore.cpp             src/core/NstNsf.cpp            src/core/NstSoundPlayer.cpp  src/core/NstVideoFilterNone.cpp

CFILES		+=	src/core/NstVideoFilterNtscCfg.c

CXXFILES	+=	src/core/vssystem/NstVsRbiBaseball.cpp  src/core/vssystem/NstVsSuperXevious.cpp  src/core/vssystem/NstVsSystem.cpp  src/core/vssystem/NstVsTkoBoxing.cpp

CXXFILES	+=	src/core/api/NstApiBarcodeReader.cpp  src/core/api/NstApiDipSwitches.cpp  src/core/api/NstApiInput.cpp    src/core/api/NstApiNsf.cpp       src/core/api/NstApiTapeRecorder.cpp		\
				src/core/api/NstApiCartridge.cpp      src/core/api/NstApiEmulator.cpp     src/core/api/NstApiMachine.cpp  src/core/api/NstApiRewinder.cpp  src/core/api/NstApiUser.cpp				\
				src/core/api/NstApiCheats.cpp         src/core/api/NstApiFds.cpp          src/core/api/NstApiMovie.cpp    src/core/api/NstApiSound.cpp     src/core/api/NstApiVideo.cpp

CXXFILES	+=	src/core/input/NstInpAdapter.cpp            src/core/input/NstInpFamilyTrainer.cpp    src/core/input/NstInpPad.cpp            src/core/input/NstInpSuborKeyboard.cpp				\
				src/core/input/NstInpBandaiHyperShot.cpp    src/core/input/NstInpHoriTrack.cpp        src/core/input/NstInpPaddle.cpp         src/core/input/NstInpTopRider.cpp						\
				src/core/input/NstInpBarcodeWorld.cpp       src/core/input/NstInpKonamiHyperShot.cpp  src/core/input/NstInpPartyTap.cpp       src/core/input/NstInpTurboFile.cpp					\
				src/core/input/NstInpCrazyClimber.cpp       src/core/input/NstInpMahjong.cpp          src/core/input/NstInpPokkunMoguraa.cpp  src/core/input/NstInpZapper.cpp						\
				src/core/input/NstInpDoremikkoKeyboard.cpp  src/core/input/NstInpMouse.cpp            src/core/input/NstInpPowerGlove.cpp															\
				src/core/input/NstInpExcitingBoxing.cpp     src/core/input/NstInpOekaKidsTablet.cpp   src/core/input/NstInpPowerPad.cpp																\
				src/core/input/NstInpFamilyKeyboard.cpp     src/core/input/NstInpPachinko.cpp         src/core/input/NstInpRob.cpp


CXXFILES	+=	src/core/board/NstBoardAe.cpp                    src/core/board/NstBoardBtlPikachuY2k.cpp          src/core/board/NstBoardNihon.cpp													\
				src/core/board/NstBoardAgci.cpp                  src/core/board/NstBoardBtlShuiGuanPipe.cpp        src/core/board/NstBoardNitra.cpp													\
				src/core/board/NstBoardAveD1012.cpp              src/core/board/NstBoardBtlSmb2a.cpp               src/core/board/NstBoardNtdec.cpp													\
				src/core/board/NstBoardAveNina.cpp               src/core/board/NstBoardBtlSmb2b.cpp               src/core/board/NstBoardOpenCorp.cpp												\
				src/core/board/NstBoardAxRom.cpp                 src/core/board/NstBoardBtlSmb2c.cpp               src/core/board/NstBoardQj.cpp													\
				src/core/board/NstBoardBandai24c0x.cpp           src/core/board/NstBoardBtlSmb3.cpp                src/core/board/NstBoardRcm.cpp													\
				src/core/board/NstBoardBandaiAerobicsStudio.cpp  src/core/board/NstBoardBtlSuperBros11.cpp         src/core/board/NstBoardRexSoftDb5z.cpp											\
				src/core/board/NstBoardBandaiDatach.cpp          src/core/board/NstBoardBtlT230.cpp                src/core/board/NstBoardRexSoftSl1632.cpp											\
				src/core/board/NstBoardBandaiKaraokeStudio.cpp   src/core/board/NstBoardBtlTobidaseDaisakusen.cpp  src/core/board/NstBoardRumbleStation.cpp											\
				src/core/board/NstBoardBandaiLz93d50.cpp         src/core/board/NstBoardBxRom.cpp                  src/core/board/NstBoardSachen74x374.cpp											\
				src/core/board/NstBoardBandaiLz93d50ex.cpp       src/core/board/NstBoardCaltron.cpp                src/core/board/NstBoardSachenS8259.cpp											\
				src/core/board/NstBoardBandaiOekaKids.cpp        src/core/board/NstBoardCamerica.cpp               src/core/board/NstBoardSachenSa0036.cpp											\
				src/core/board/NstBoardBenshengBs5.cpp           src/core/board/NstBoardCneDecathlon.cpp           src/core/board/NstBoardSachenSa0037.cpp											\
				src/core/board/NstBoardBmc110in1.cpp             src/core/board/NstBoardCnePsb.cpp                 src/core/board/NstBoardSachenSa72007.cpp											\
				src/core/board/NstBoardBmc1200in1.cpp            src/core/board/NstBoardCneShlz.cpp                src/core/board/NstBoardSachenSa72008.cpp											\
				src/core/board/NstBoardBmc150in1.cpp             src/core/board/NstBoardCony.cpp                   src/core/board/NstBoardSachenStreetHeroes.cpp									\
				src/core/board/NstBoardBmc15in1.cpp              src/core/board/NstBoard.cpp                       src/core/board/NstBoardSachenTca01.cpp											\
				src/core/board/NstBoardBmc20in1.cpp              src/core/board/NstBoardCxRom.cpp                  src/core/board/NstBoardSachenTcu.cpp												\
				src/core/board/NstBoardBmc21in1.cpp              src/core/board/NstBoardDiscrete.cpp               src/core/board/NstBoardSomeriTeamSl12.cpp										\
				src/core/board/NstBoardBmc22Games.cpp            src/core/board/NstBoardDreamtech.cpp              src/core/board/NstBoardSubor.cpp													\
				src/core/board/NstBoardBmc31in1.cpp              src/core/board/NstBoardEvent.cpp                  src/core/board/NstBoardSunsoft1.cpp												\
				src/core/board/NstBoardBmc35in1.cpp              src/core/board/NstBoardFb.cpp                     src/core/board/NstBoardSunsoft2.cpp												\
				src/core/board/NstBoardBmc36in1.cpp              src/core/board/NstBoardFfe.cpp                    src/core/board/NstBoardSunsoft3.cpp												\
				src/core/board/NstBoardBmc64in1.cpp              src/core/board/NstBoardFujiya.cpp                 src/core/board/NstBoardSunsoft4.cpp												\
				src/core/board/NstBoardBmc72in1.cpp              src/core/board/NstBoardFukutake.cpp               src/core/board/NstBoardSunsoft5b.cpp												\
				src/core/board/NstBoardBmc76in1.cpp              src/core/board/NstBoardFutureMedia.cpp            src/core/board/NstBoardSunsoftDcs.cpp											\
				src/core/board/NstBoardBmc800in1.cpp             src/core/board/NstBoardGouder.cpp                 src/core/board/NstBoardSunsoftFme7.cpp											\
				src/core/board/NstBoardBmc8157.cpp               src/core/board/NstBoardGxRom.cpp                  src/core/board/NstBoardSuperGameBoogerman.cpp									\
				src/core/board/NstBoardBmc9999999in1.cpp         src/core/board/NstBoardHenggedianzi.cpp           src/core/board/NstBoardSuperGameLionKing.cpp										\
				src/core/board/NstBoardBmcA65as.cpp              src/core/board/NstBoardHes.cpp                    src/core/board/NstBoardSuperGamePocahontas2.cpp									\
				src/core/board/NstBoardBmcBallgames11in1.cpp     src/core/board/NstBoardHosenkan.cpp               src/core/board/NstBoardTaitoTc0190fmc.cpp										\
				src/core/board/NstBoardBmcCh001.cpp              src/core/board/NstBoardIremG101.cpp               src/core/board/NstBoardTaitoTc0190fmcPal16r4.cpp									\
				src/core/board/NstBoardBmcCtc65.cpp              src/core/board/NstBoardIremH3001.cpp              src/core/board/NstBoardTaitoX1005.cpp											\
				src/core/board/NstBoardBmcFamily4646B.cpp        src/core/board/NstBoardIremHolyDiver.cpp          src/core/board/NstBoardTaitoX1017.cpp											\
				src/core/board/NstBoardBmcFk23c.cpp              src/core/board/NstBoardIremKaiketsu.cpp           src/core/board/NstBoardTengen.cpp												\
				src/core/board/NstBoardBmcGamestarA.cpp          src/core/board/NstBoardIremLrog017.cpp            src/core/board/NstBoardTengenRambo1.cpp											\
				src/core/board/NstBoardBmcGamestarB.cpp          src/core/board/NstBoardJalecoJf11.cpp             src/core/board/NstBoardTxc.cpp													\
				src/core/board/NstBoardBmcGolden190in1.cpp       src/core/board/NstBoardJalecoJf13.cpp             src/core/board/NstBoardTxcMxmdhtwo.cpp											\
				src/core/board/NstBoardBmcGoldenCard6in1.cpp     src/core/board/NstBoardJalecoJf16.cpp             src/core/board/NstBoardTxcPoliceman.cpp											\
				src/core/board/NstBoardBmcGoldenGame260in1.cpp   src/core/board/NstBoardJalecoJf17.cpp             src/core/board/NstBoardTxcTw.cpp													\
				src/core/board/NstBoardBmcHero.cpp               src/core/board/NstBoardJalecoJf19.cpp             src/core/board/NstBoardTxRom.cpp													\
				src/core/board/NstBoardBmcMarioParty7in1.cpp     src/core/board/NstBoardJalecoSs88006.cpp          src/core/board/NstBoardUnlA9746.cpp												\
				src/core/board/NstBoardBmcNovelDiamond.cpp       src/core/board/NstBoardJyCompany.cpp              src/core/board/NstBoardUnlCc21.cpp												\
				src/core/board/NstBoardBmcPowerjoy84in1.cpp      src/core/board/NstBoardKaiser.cpp                 src/core/board/NstBoardUnlEdu2000.cpp											\
				src/core/board/NstBoardBmcResetBased4in1.cpp     src/core/board/NstBoardKasing.cpp                 src/core/board/NstBoardUnlKingOfFighters96.cpp									\
				src/core/board/NstBoardBmcSuper22Games.cpp       src/core/board/NstBoardKayH2288.cpp               src/core/board/NstBoardUnlKingOfFighters97.cpp									\
				src/core/board/NstBoardBmcSuper24in1.cpp         src/core/board/NstBoardKayPandaPrince.cpp         src/core/board/NstBoardUnlMortalKombat2.cpp										\
				src/core/board/NstBoardBmcSuper40in1.cpp         src/core/board/NstBoardKonamiVrc1.cpp             src/core/board/NstBoardUnlN625092.cpp											\
				src/core/board/NstBoardBmcSuper700in1.cpp        src/core/board/NstBoardKonamiVrc2.cpp             src/core/board/NstBoardUnlSuperFighter3.cpp										\
				src/core/board/NstBoardBmcSuperBig7in1.cpp       src/core/board/NstBoardKonamiVrc3.cpp             src/core/board/NstBoardUnlTf1201.cpp												\
				src/core/board/NstBoardBmcSuperGun20in1.cpp      src/core/board/NstBoardKonamiVrc4.cpp             src/core/board/NstBoardUnlWorldHero.cpp											\
				src/core/board/NstBoardBmcSuperHiK300in1.cpp     src/core/board/NstBoardKonamiVrc6.cpp             src/core/board/NstBoardUnlXzy.cpp												\
				src/core/board/NstBoardBmcSuperHiK4in1.cpp       src/core/board/NstBoardKonamiVrc7.cpp             src/core/board/NstBoardUxRom.cpp													\
				src/core/board/NstBoardBmcSuperVision16in1.cpp   src/core/board/NstBoardKonamiVsSystem.cpp         src/core/board/NstBoardVsSystem.cpp												\
				src/core/board/NstBoardBmcT262.cpp               src/core/board/NstBoardMagicSeries.cpp            src/core/board/NstBoardWaixing.cpp												\
				src/core/board/NstBoardBmcVrc4.cpp               src/core/board/NstBoardMmc1.cpp                   src/core/board/NstBoardWaixingFfv.cpp											\
				src/core/board/NstBoardBmcVt5201.cpp             src/core/board/NstBoardMmc2.cpp                   src/core/board/NstBoardWaixingPs2.cpp											\
				src/core/board/NstBoardBmcY2k64in1.cpp           src/core/board/NstBoardMmc3.cpp                   src/core/board/NstBoardWaixingSecurity.cpp										\
				src/core/board/NstBoardBtl2708.cpp               src/core/board/NstBoardMmc4.cpp                   src/core/board/NstBoardWaixingSgz.cpp											\
				src/core/board/NstBoardBtl6035052.cpp            src/core/board/NstBoardMmc5.cpp                   src/core/board/NstBoardWaixingSgzlz.cpp											\
				src/core/board/NstBoardBtlAx5705.cpp             src/core/board/NstBoardMmc6.cpp                   src/core/board/NstBoardWaixingSh2.cpp											\
				src/core/board/NstBoardBtlDragonNinja.cpp        src/core/board/NstBoardNamcot163.cpp              src/core/board/NstBoardWaixingZs.cpp												\
				src/core/board/NstBoardBtlGeniusMerioBros.cpp    src/core/board/NstBoardNamcot34xx.cpp             src/core/board/NstBoardWhirlwind.cpp												\
				src/core/board/NstBoardBtlMarioBaby.cpp          src/core/board/NstBoardNanjing.cpp                src/core/board/NstBoardZz.cpp


include $(CURDIR)/../Makefile-$(PLATFORM).base

