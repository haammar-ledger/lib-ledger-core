// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from addresses.djinni

package co.ledger.core;

public final class BitcoinLikeNetworkParameters {


    /*package*/ final String Identifier;

    /*package*/ final byte[] P2PKHVersion;

    /*package*/ final byte[] P2SHVersion;

    /*package*/ final byte[] XPUBVersion;

    /*package*/ final BitcoinLikeFeePolicy FeePolicy;

    /*package*/ final long BIP44CoinType;

    /*package*/ final String PaymentUriScheme;

    /*package*/ final long DustAmount;

    /*package*/ final String MessagePrefix;

    /*package*/ final boolean UsesTimestampedTransaction;

    public BitcoinLikeNetworkParameters(
            String Identifier,
            byte[] P2PKHVersion,
            byte[] P2SHVersion,
            byte[] XPUBVersion,
            BitcoinLikeFeePolicy FeePolicy,
            long BIP44CoinType,
            String PaymentUriScheme,
            long DustAmount,
            String MessagePrefix,
            boolean UsesTimestampedTransaction) {
        this.Identifier = Identifier;
        this.P2PKHVersion = P2PKHVersion;
        this.P2SHVersion = P2SHVersion;
        this.XPUBVersion = XPUBVersion;
        this.FeePolicy = FeePolicy;
        this.BIP44CoinType = BIP44CoinType;
        this.PaymentUriScheme = PaymentUriScheme;
        this.DustAmount = DustAmount;
        this.MessagePrefix = MessagePrefix;
        this.UsesTimestampedTransaction = UsesTimestampedTransaction;
    }

    public String getIdentifier() {
        return Identifier;
    }

    public byte[] getP2PKHVersion() {
        return P2PKHVersion;
    }

    public byte[] getP2SHVersion() {
        return P2SHVersion;
    }

    public byte[] getXPUBVersion() {
        return XPUBVersion;
    }

    public BitcoinLikeFeePolicy getFeePolicy() {
        return FeePolicy;
    }

    public long getBIP44CoinType() {
        return BIP44CoinType;
    }

    public String getPaymentUriScheme() {
        return PaymentUriScheme;
    }

    public long getDustAmount() {
        return DustAmount;
    }

    public String getMessagePrefix() {
        return MessagePrefix;
    }

    public boolean getUsesTimestampedTransaction() {
        return UsesTimestampedTransaction;
    }

    @Override
    public String toString() {
        return "BitcoinLikeNetworkParameters{" +
                "Identifier=" + Identifier +
                "," + "P2PKHVersion=" + P2PKHVersion +
                "," + "P2SHVersion=" + P2SHVersion +
                "," + "XPUBVersion=" + XPUBVersion +
                "," + "FeePolicy=" + FeePolicy +
                "," + "BIP44CoinType=" + BIP44CoinType +
                "," + "PaymentUriScheme=" + PaymentUriScheme +
                "," + "DustAmount=" + DustAmount +
                "," + "MessagePrefix=" + MessagePrefix +
                "," + "UsesTimestampedTransaction=" + UsesTimestampedTransaction +
        "}";
    }

}
