#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

#include <evse_security/crypto/openssl/openssl_supplier.hpp>

using namespace evse_security;

namespace {

static std::string getFile(const std::string name) {
    std::ifstream file(name);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

class OpenSSLSupplierTest : public testing::Test {
protected:
    static void SetUpTestSuite() {
        std::system("./create-pki.sh");
    }
};

TEST_F(OpenSSLSupplierTest, generate_key_RSA_TPM20) {
    KeyGenerationInfo info = {
        CryptoKeyType::RSA_TPM20, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_RSA_3072) {
    KeyGenerationInfo info = {
        CryptoKeyType::RSA_3072, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_RSA_7680) {
    KeyGenerationInfo info = {
        CryptoKeyType::RSA_7680, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_EC_prime256v1) {
    KeyGenerationInfo info = {
        CryptoKeyType::EC_prime256v1, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_EC_EC_secp384r1) {
    KeyGenerationInfo info = {
        CryptoKeyType::EC_secp384r1, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, load_certificates) {
    auto file = getFile("pki/cert_path.pem");
    auto res = OpenSSLSupplier::load_certificates(file, EncodingFormat::PEM);
    ASSERT_EQ(res.size(), 2);
}

TEST_F(OpenSSLSupplierTest, x509_check_private_key) {
    auto cert_leaf = getFile("pki/server_cert.pem");
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);
    auto cert = res_leaf[0].get();
    auto key = getFile("pki/server_priv.pem");
    auto res = OpenSSLSupplier::x509_check_private_key(cert, key, std::nullopt);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, x509_verify_certificate_chain) {
    auto cert_path = getFile("pki/cert_path.pem");
    auto cert_leaf = getFile("pki/server_cert.pem");

    auto res_path = OpenSSLSupplier::load_certificates(cert_path, EncodingFormat::PEM);
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);

    std::vector<X509Handle*> parents;

    for (auto& i : res_path) {
        parents.push_back(i.get());
    }

    auto res = OpenSSLSupplier::x509_verify_certificate_chain(res_leaf[0].get(), parents, true, std::nullopt,
                                                              "pki/root_cert.pem");
    ASSERT_EQ(res, CertificateValidationError::NoError);
}

TEST_F(OpenSSLSupplierTest, x509_generate_csr) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        {CryptoKeyType::EC_prime256v1, false, std::nullopt, "pki/csr_key.pem", std::nullopt}};
    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);
    ASSERT_TRUE(res);

    ASSERT_GT(csr.size(), 0);
}

} // namespace
