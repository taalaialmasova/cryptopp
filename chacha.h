// chacha.h - written and placed in the public domain by Jeffrey Walton.
//            Based on Wei Dai's Salsa20, Botan's SSE2 implementation,
//            and Bernstein's reference ChaCha family implementation at
//            http://cr.yp.to/chacha.html.

// The library added Bernstein's ChaCha classses at Crypto++ 5.6.4. The IETF
// uses a slightly different implementation than Bernstein, and the IETF
// classes were added at Crypto++ 8.1. We wanted to maintain ABI compatibility
// at the 8.1 release so the original ChaCha classes were not disturbed.
// Instead new classes were added for IETF ChaCha. The back-end implementation
// shares code as expected, however.

/// \file chacha.h
/// \brief Classes for ChaCha8, ChaCha12 and ChaCha20 stream ciphers
/// \details Crypto++ provides Bernstein and ECRYPT's ChaCha from <a href="http://cr.yp.to/chacha/chacha-20080128.pdf">ChaCha,
///   a variant of Salsa20</a> (2008.01.28). Crypto++ also provides the IETF
///   implementation of ChaCha using the ChaChaTLS name. Bernstein's
///   implementation is _slightly_ different from the TLS working group's
///   implementation for cipher suites
///   <tt>TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256</tt>,
///   <tt>TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256</tt>,
///   and <tt>TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256</tt>.
/// \since ChaCha since Crypto++ 5.6.4, ChaChaTLS since Crypto++ 8.1

#ifndef CRYPTOPP_CHACHA_H
#define CRYPTOPP_CHACHA_H

#include "strciphr.h"
#include "secblock.h"

NAMESPACE_BEGIN(CryptoPP)

////////////////////////////// Bernstein ChaCha //////////////////////////////

/// \brief ChaCha stream cipher information
/// \since Crypto++ 5.6.4
struct ChaCha_Info : public VariableKeyLength<32, 16, 32, 16, SimpleKeyingInterface::UNIQUE_IV, 8>
{
    /// \brief The algorithm name
    /// \returns the algorithm name
    /// \details StaticAlgorithmName returns the algorithm's name as a static
    ///   member function.
    /// \details Bernstein named the cipher variants ChaCha8, ChaCha12 and
    ///   ChaCha20. More generally, Bernstein called the family ChaCha{r}.
    ///   AlgorithmName() provides the exact name once rounds are set.
    static const char* StaticAlgorithmName() {
        return "ChaCha";
    }
};

/// \brief ChaCha stream cipher implementation
/// \since Crypto++ 5.6.4
class CRYPTOPP_NO_VTABLE ChaCha_Policy : public AdditiveCipherConcretePolicy<word32, 16>
{
public:
    virtual ~ChaCha_Policy() {}
    ChaCha_Policy() : m_rounds(0) {}

protected:
    void CipherSetKey(const NameValuePairs &params, const byte *key, size_t length);
    void OperateKeystream(KeystreamOperation operation, byte *output, const byte *input, size_t iterationCount);
    void CipherResynchronize(byte *keystreamBuffer, const byte *IV, size_t length);
    bool CipherIsRandomAccess() const {return true;}
    void SeekToIteration(lword iterationCount);
    unsigned int GetAlignment() const;
    unsigned int GetOptimalBlockSize() const;

    std::string AlgorithmName() const;
    std::string AlgorithmProvider() const;

    FixedSizeAlignedSecBlock<word32, 16> m_state;
    unsigned int m_rounds;
};

/// \brief ChaCha stream cipher
/// \details This is Bernstein and ECRYPT's ChaCha. It is _slightly_ different
///   from the IETF's version of ChaCha called ChaChaTLS.
/// \sa <a href="http://cr.yp.to/chacha/chacha-20080208.pdf">ChaCha, a variant
///   of Salsa20</a> (2008.01.28).
/// \since Crypto++ 5.6.4
struct ChaCha : public ChaCha_Info, public SymmetricCipherDocumentation
{
    typedef SymmetricCipherFinal<ConcretePolicyHolder<ChaCha_Policy, AdditiveCipherTemplate<> >, ChaCha_Info > Encryption;
    typedef Encryption Decryption;
};

////////////////////////////// IETF ChaChaTLS //////////////////////////////

/// \brief ChaCha-TLS stream cipher information
/// \since Crypto++ 8.1
struct ChaChaTLS_Info : public FixedKeyLength<32, SimpleKeyingInterface::UNIQUE_IV, 12>, FixedRounds<20>
{
    /// \brief The algorithm name
    /// \returns the algorithm name
    /// \details StaticAlgorithmName returns the algorithm's name as a static
    ///   member function.
    /// \details This is the IETF's variant of Bernstein's ChaCha from RFC
    ///   8439. IETF ChaCha is called ChaChaTLS in the Crypto++ library. It
    ///   is _slightly_ different from Bernstein's implementation.
    static const char* StaticAlgorithmName() {
        return "ChaChaTLS";
    }
};

/// \brief ChaCha-TLS stream cipher implementation
/// \since Crypto++ 8.1
class CRYPTOPP_NO_VTABLE ChaChaTLS_Policy : public AdditiveCipherConcretePolicy<word32, 16>
{
public:
    virtual ~ChaChaTLS_Policy() {}
    ChaChaTLS_Policy() {}

protected:
    void CipherSetKey(const NameValuePairs &params, const byte *key, size_t length);
    void OperateKeystream(KeystreamOperation operation, byte *output, const byte *input, size_t iterationCount);
    void CipherResynchronize(byte *keystreamBuffer, const byte *IV, size_t length);
	void CipherResynchronize(byte *keystreamBuffer, word32 initialBlock, const byte *IV, size_t length);
    bool CipherIsRandomAccess() const {return true;}
    void SeekToIteration(lword iterationCount);
    unsigned int GetAlignment() const;
    unsigned int GetOptimalBlockSize() const;

    std::string AlgorithmName() const;
    std::string AlgorithmProvider() const;

    FixedSizeAlignedSecBlock<word32, 16+1> m_state;
    CRYPTOPP_CONSTANT(m_rounds = ChaChaTLS_Info::ROUNDS)
};

/// \brief ChaCha-TLS stream cipher
/// \details This is the IETF's variant of Bernstein's ChaCha from RFC 8439.
///   IETF ChaCha is called ChaChaTLS in the Crypto++ library. It is
///   _slightly_ different from the Bernstein implementation. ChaCha-TLS
///   can be used for cipher suites
///   <tt>TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256</tt>,
///   <tt>TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256</tt>, and
///   <tt>TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256</tt>.
/// \sa <a href="https://tools.ietf.org/html/rfc8439">ChaCha20 and Poly1305 for
///   IETF Protocols</a> and <A HREF="https://github.com/weidai11/cryptopp/issues/790">Issue
///   790, ChaChaTLS results when counter block wraps</A>.
/// \since Crypto++ 8.1
struct ChaChaTLS : public ChaChaTLS_Info, public SymmetricCipherDocumentation
{
    typedef SymmetricCipherFinal<ConcretePolicyHolder<ChaChaTLS_Policy, AdditiveCipherTemplate<> >, ChaChaTLS_Info > Encryption;
    typedef Encryption Decryption;
};

NAMESPACE_END

#endif  // CRYPTOPP_CHACHA_H
