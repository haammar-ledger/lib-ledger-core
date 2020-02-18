#pragma once

#include <unordered_set>

#include <gtest/gtest.h>

#include <soci.h>

#include <async/QtThreadDispatcher.hpp>
#include <net/QtHttpClient.hpp>
#include <async/AsyncWait.hpp>

#include <NativePathResolver.hpp>
#include <CoutLogPrinter.hpp>

#include <core/database/DatabaseSessionPool.hpp>
#include <core/Services.hpp>
#include <core/api/DynamicObject.hpp>
#include <core/api/BigInt.hpp>
#include <core/api/Account.hpp>
#include <core/wallet/CurrencyBuilder.hpp>
#include <core/wallet/AccountDatabaseHelper.hpp>
#include <core/utils/JSONUtils.hpp>
#include <core/events/LambdaEventReceiver.hpp>

#include <bitcoin/BitcoinLikeWallet.hpp>
#include <bitcoin/BitcoinLikeAccount.hpp>
#include <bitcoin/api/BitcoinLikeOperation.hpp>
#include <bitcoin/api/BitcoinLikeTransaction.hpp>
#include <bitcoin/api/BitcoinLikeInput.hpp>
#include <bitcoin/api/BitcoinLikeAccount.hpp>
#include <bitcoin/api/BitcoinLikeOutput.hpp>
#include <bitcoin/database/BitcoinLikeWalletDatabase.hpp>
#include <bitcoin/database/BitcoinLikeTransactionDatabaseHelper.hpp>
#include <bitcoin/transactions/BitcoinLikeTransactionParser.hpp>

namespace ledger {
	namespace testing {
		namespace medium_xpub {
			extern core::api::ExtendedKeyAccountCreationInfo XPUB_INFO;
			extern const std::string TX_1;
			extern const std::string TX_2;
			extern const std::string TX_3;
			extern const std::string TX_4;
			extern const std::string TX_5;
			extern const std::string TX_6;
			extern const std::string TX_7;
			extern const std::string TX_8;
			extern const std::string TX_9;
			extern const std::string TX_10;
			extern const std::string TX_11;
			extern const std::string TX_12;
			extern const std::string TX_13;
			extern const std::string TX_14;
			extern const std::string TX_15;
			extern const std::string TX_16;
			extern const std::string TX_17;
			extern const std::string TX_18;
			extern const std::string TX_19;
			extern const std::string TX_20;
			extern const std::string TX_21;
			extern const std::string TX_22;
			extern const std::string TX_23;
			extern const std::string TX_24;
			extern const std::string TX_25;
			extern const std::string TX_26;
			extern const std::string TX_27;
			extern const std::string TX_28;
			extern const std::string TX_29;
			extern const std::string TX_30;
			extern const std::string TX_31;
			extern const std::string TX_32;
			extern const std::string TX_33;
			extern const std::string TX_34;
			extern const std::string TX_35;
			extern const std::string TX_36;
			extern const std::string TX_37;
			extern const std::string TX_38;
			extern const std::string TX_39;
			extern const std::string TX_40;
			extern const std::string TX_41;
			extern const std::string TX_42;
			extern const std::string TX_43;
			extern const std::string TX_44;
			extern const std::string TX_45;
			extern const std::string TX_46;
			extern const std::string TX_47;
			extern const std::string TX_48;
			extern const std::string TX_49;
			extern const std::string TX_50;
			extern const std::string TX_51;
			extern const std::string TX_52;
			extern const std::string TX_53;
			extern const std::string TX_54;
			extern const std::string TX_55;
			extern const std::string TX_56;
			extern const std::string TX_57;
			extern const std::string TX_58;
			extern const std::string TX_59;
			extern const std::string TX_60;
			extern const std::string TX_61;
			extern const std::string TX_62;
			extern const std::string TX_63;
			extern const std::string TX_64;
			extern const std::string TX_65;
			extern const std::string TX_66;
			extern const std::string TX_67;
			extern const std::string TX_68;
			extern const std::string TX_69;
			extern const std::string TX_70;
			extern const std::string TX_71;
			extern const std::string TX_72;
			extern const std::string TX_73;
			extern const std::string TX_74;
			extern const std::string TX_75;
			extern const std::string TX_76;
			extern const std::string TX_77;
			extern const std::string TX_78;
			extern const std::string TX_79;
			extern const std::string TX_80;
			extern const std::string TX_81;
			extern const std::string TX_82;
			extern const std::string TX_83;
			extern const std::string TX_84;
			extern const std::string TX_85;
			extern const std::string TX_86;
			extern const std::string TX_87;
			extern const std::string TX_88;
			extern const std::string TX_89;
			extern const std::string TX_90;
			extern const std::string TX_91;
			extern const std::string TX_92;
			extern const std::string TX_93;
			extern const std::string TX_94;
			extern const std::string TX_95;
			extern const std::string TX_96;
			extern const std::string TX_97;
			extern const std::string TX_98;
			extern const std::string TX_99;
			extern const std::string TX_100;
			extern const std::string TX_101;
			extern const std::string TX_102;
			extern const std::string TX_103;
			extern const std::string TX_104;
			extern const std::string TX_105;
			extern const std::string TX_106;
			extern const std::string TX_107;
			extern const std::string TX_108;
			extern const std::string TX_109;
			extern const std::string TX_110;
			extern const std::string TX_111;
			extern const std::string TX_112;
			extern const std::string TX_113;
			extern const std::string TX_114;
			extern const std::string TX_115;
			extern const std::string TX_116;
			extern const std::string TX_117;
			extern const std::string TX_118;
			extern const std::string TX_119;
			extern const std::string TX_120;
			extern const std::string TX_121;
			extern const std::string TX_122;
			extern const std::string TX_123;
			extern const std::string TX_124;
			extern const std::string TX_125;
			extern const std::string TX_126;
			extern const std::string TX_127;
			extern const std::string TX_128;
			extern const std::string TX_129;
			extern const std::string TX_130;
			extern const std::string TX_131;
			extern const std::string TX_132;
			extern const std::string TX_133;
			extern const std::string TX_134;
			extern const std::string TX_135;
			extern const std::string TX_136;
			extern const std::string TX_137;
			extern const std::string TX_138;
			extern const std::string TX_139;
			extern const std::string TX_140;
			extern const std::string TX_141;
			extern const std::string TX_142;
			extern const std::string TX_143;
			extern const std::string TX_144;
			extern const std::string TX_145;
			extern const std::string TX_146;
			extern const std::string TX_147;
			extern const std::string TX_148;
			extern const std::string TX_149;
			extern const std::string TX_150;
			extern const std::string TX_151;
			extern const std::string TX_152;
			extern const std::string TX_153;
			extern const std::string TX_154;
			extern const std::string TX_155;
			extern const std::string TX_156;
			extern const std::string TX_157;
			extern const std::string TX_158;
			extern const std::string TX_159;
			extern const std::string TX_160;
			extern const std::string TX_161;
			extern const std::string TX_162;
			extern const std::string TX_163;
			extern const std::string TX_164;
			extern const std::string TX_165;
			extern const std::string TX_166;
			extern const std::string TX_167;
			extern const std::string TX_168;
			extern const std::string TX_169;
			extern const std::string TX_170;
			extern const std::string TX_171;
			extern const std::string TX_172;
			extern const std::string TX_173;
			extern const std::string TX_174;
			extern const std::string TX_175;
			extern const std::string TX_176;
			extern const std::string TX_177;
			extern const std::string TX_178;
			extern const std::string TX_179;
			extern const std::string TX_180;
			extern const std::string TX_181;
			extern const std::string TX_182;
			extern const std::string TX_183;
			extern const std::string TX_184;
			extern const std::string TX_185;
			extern const std::string TX_186;
			extern const std::string TX_187;
			extern const std::string TX_188;
			extern const std::string TX_189;
			extern const std::string TX_190;
			extern const std::string TX_191;
			extern const std::string TX_192;
			extern const std::string TX_193;
			extern const std::string TX_194;
			extern const std::string TX_195;
			extern const std::string TX_196;
			extern const std::string TX_197;
			extern const std::string TX_198;
			extern const std::string TX_199;
			extern const std::string TX_200;
			extern const std::string TX_201;
			extern const std::string TX_202;
			extern const std::string TX_203;
			extern const std::string TX_204;
			extern const std::string TX_205;
			extern const std::string TX_206;
			extern const std::string TX_207;
			extern const std::string TX_208;
			extern const std::string TX_209;
			extern const std::string TX_210;
			extern const std::string TX_211;
			extern const std::string TX_212;
			extern const std::string TX_213;
			extern const std::string TX_214;
			extern const std::string TX_215;
			extern const std::string TX_216;
			extern const std::string TX_217;
			extern const std::string TX_218;
			extern const std::string TX_219;
			extern const std::string TX_220;
			extern const std::string TX_221;
			extern const std::string TX_222;
			extern const std::string TX_223;
			extern const std::string TX_224;
			extern const std::string TX_225;
			extern const std::string TX_226;
			extern const std::string TX_227;
			extern const std::string TX_228;
			extern const std::string TX_229;
			extern const std::string TX_230;
			extern const std::string TX_231;
			extern const std::string TX_232;
			extern const std::string TX_233;
			extern const std::string TX_234;
			extern const std::string TX_235;
			extern const std::string TX_236;
			extern const std::string TX_237;
			extern const std::string TX_238;
			extern const std::string TX_239;
			extern const std::string TX_240;
			extern const std::string TX_241;
			extern const std::string TX_242;
			extern const std::string TX_243;
			extern const std::string TX_244;
			extern const std::string TX_245;
			extern const std::string TX_246;
			extern const std::string TX_247;
			extern const std::string TX_248;
			extern const std::string TX_249;
			extern const std::string TX_250;
			extern const std::string TX_251;
			extern const std::string TX_252;
			extern const std::string TX_253;
			extern const std::string TX_254;
			extern const std::string TX_255;
			extern const std::string TX_256;
			extern const std::string TX_257;
			extern const std::string TX_258;
			extern const std::string TX_259;
			extern const std::string TX_260;
			extern const std::string TX_261;
			extern const std::string TX_262;
			extern const std::string TX_263;
			extern const std::string TX_264;
			extern const std::string TX_265;
			extern const std::string TX_266;
			extern const std::string TX_267;
			extern const std::string TX_268;
			extern const std::string TX_269;
			extern const std::string TX_270;
			extern const std::string TX_271;
			extern const std::string TX_272;
			extern const std::string TX_273;
			extern const std::string TX_274;
			extern const std::string TX_275;
			extern const std::string TX_276;
			extern const std::string TX_277;
			extern const std::string TX_278;
			extern const std::string TX_279;
			extern const std::string TX_280;
			extern const std::string TX_281;
			extern const std::string TX_282;
			extern const std::string TX_283;
			extern const std::string TX_284;
			extern const std::string TX_285;
			extern const std::string TX_286;
			extern const std::string TX_287;
			extern const std::string TX_288;
			extern const std::string TX_289;
			extern const std::string TX_290;
			extern const std::string TX_291;
			extern const std::string TX_292;
			extern const std::string TX_293;
			extern const std::string TX_294;
			extern const std::string TX_295;
			extern const std::string TX_296;
			extern const std::string TX_297;
			extern const std::string TX_298;
			extern const std::string TX_299;
			extern const std::string TX_300;
			extern const std::string TX_301;
			extern const std::string TX_302;
			extern const std::string TX_303;
			extern const std::string TX_304;
			extern const std::string TX_305;
			extern const std::string TX_306;
			extern const std::string TX_307;
			extern const std::string TX_308;
			extern const std::string TX_309;
			extern const std::string TX_310;
			extern const std::string TX_311;
			extern const std::string TX_312;
			extern const std::string TX_313;
			extern const std::string TX_314;
			extern const std::string TX_315;
			extern const std::string TX_316;
			extern const std::string TX_317;
			extern const std::string TX_318;
			extern const std::string TX_319;
			extern const std::string TX_320;
			extern const std::string TX_321;
			extern const std::string TX_322;
			extern const std::string TX_323;
			extern const std::string TX_324;
			extern const std::string TX_325;
			extern const std::string TX_326;
			extern const std::string TX_327;
			extern const std::string TX_328;
			extern const std::string TX_329;
			extern const std::string TX_330;
			extern const std::string TX_331;
			extern const std::string TX_332;
			extern const std::string TX_333;
			extern const std::string TX_334;
			extern const std::string TX_335;

			std::shared_ptr<core::BitcoinLikeAccount> inflate(const std::shared_ptr<core::Services>& services, const std::shared_ptr<core::AbstractWallet>& wallet);
		}
	}
}
