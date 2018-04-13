// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from addresses.djinni

#import "LGBitcoinLikeNetworkParameters.h"
#import <Foundation/Foundation.h>
@class LGBitcoinLikeAddress;
@class LGBitcoinLikeExtendedPublicKey;


@interface LGBitcoinLikeExtendedPublicKey : NSObject

- (nullable LGBitcoinLikeAddress *)derive:(nonnull NSString *)path;

- (nonnull NSData *)derivePublicKey:(nonnull NSString *)path;

- (nonnull NSData *)deriveHash160:(nonnull NSString *)path;

- (nonnull NSString *)toBase58;

- (nonnull NSString *)getRootPath;

+ (nullable LGBitcoinLikeExtendedPublicKey *)fromBase58:(nonnull LGBitcoinLikeNetworkParameters *)params
                                                address:(nonnull NSString *)address
                                                   path:(nullable NSString *)path;

@end
