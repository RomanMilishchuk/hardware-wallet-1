/*
 * This file is part of the Skycoin project, https://skycoin.net/
 *
 * Copyright (C) 2018-2019 Skycoin Project
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef SKYCOIN_CRYPTO_H
#define SKYCOIN_CRYPTO_H

#include "tools/sha2.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct TransactionOutput {
    uint64_t coin;
    uint64_t hour;
    uint8_t address[20];
    uint32_t address_n[8];
} TransactionOutput;

typedef struct Transaction {
    uint8_t nbIn;
    uint8_t nbOut;
    uint8_t inAddress[8][32];
    TransactionOutput outAddress[8];
    int has_innerHash;
    uint8_t innerHash[32];
} Transaction;

typedef enum {
    Destroyed,
    Start,
    InnerHashInputs,
    InnerHashOutputs,
    Signature,
} TxSignState;

typedef struct _TxSignContext {
    bool mnemonic_change;
    TxSignState state;
    uint8_t nbIn;
    uint8_t nbOut;
    uint8_t current_nbIn;
    uint8_t current_nbOut;
    char coin_name[36];
    uint32_t version;
    uint32_t lock_time;
    char tx_hash[65];
    bool has_innerHash;
    uint8_t innerHash[32];
    uint64_t requestIndex;
    SHA256_CTX sha256_ctx;
} TxSignContext;

void transaction_initZeroTransaction(Transaction* self);
void transaction_addInput(Transaction* self, uint8_t* address);
void transaction_addOutput(Transaction* self, uint32_t coin, uint32_t hour, char* address);
void transaction_innerHash(Transaction* self);
void transaction_msgToSign(Transaction* self, uint8_t index, uint8_t* signature);

int ecdh(const uint8_t* pub_key, const uint8_t* sec_key, uint8_t* ecdh_key);
int secp256k1sum(const uint8_t* seed, const size_t seed_length, uint8_t* digest);
void sha256sum(const uint8_t* seed, uint8_t* digest, size_t seed_length);
void sha256sum_two(const uint8_t* msg1, size_t msg1_len, const uint8_t* msg2, size_t msg2_len, uint8_t* out_digest);
int deterministic_key_pair_iterator(const uint8_t* seed, const size_t seed_length, uint8_t* nextSeed, uint8_t* seckey, uint8_t* pubkey);
int deterministic_key_pair_iterator_step(const uint8_t* seed, uint8_t* seckey, uint8_t* pubkey);
void skycoin_pubkey_from_seckey(const uint8_t* seckey, uint8_t* pubkey);
int skycoin_address_from_pubkey(const uint8_t* pubkey, char* address, size_t* size_address);
int skycoin_ecdsa_sign_digest(const uint8_t* priv_key, const uint8_t* digest, uint8_t* sig);
void tohex(char* str, const uint8_t* buffer, int buffer_length);
void tobuff(const char* str, uint8_t* buf, size_t buffer_length);
void writebuf_fromhexstr(const char* str, uint8_t* buf);

/*  @brief Initialize global variable context
 *  @return Pointer to TxSignContext
 */
TxSignContext* TxSignCtx_Init(void);
/* @brief Get current transaction context 
 * @return Pointer to TxSignContext
 */
TxSignContext* TxSignCtx_Get(void);
/*  @brief Add size prefix of inputs or outputs to SHA256 context inside ctx
 *  @param ctx Context with SHA256 context for update
 *  @param nbIn Number of inputs or outputs
 */
void TxSignCtx_AddSizePrefix(TxSignContext* ctx, uint8_t nbIn);
/*  @brief Update SHA256 context with new inputs
 *  @param ctx Context with SHA256 context to be updated
 *  @param inputs Inputs for add
 *  @param count Number of inputs on inputs param
 */
void TxSignCtx_UpdateInputs(TxSignContext* ctx, uint8_t inputs [7][32], uint8_t count);
/*  @brief Update SHA256 context with new outputs
 *  @param ctx Context with SHA256 context to be updated
 *  @param outputs Outputs for add
 *  @param count Number of outputs on outputs param
 */
void TxSignCtx_UpdateOutputs(TxSignContext* ctx, TransactionOutput outputs[7], uint8_t count);
/*  @brief Finish SHA256 context and store inner hash on innerHash and set has_innerHash correctly
 *  @param ctx Context with SHA256 to be finished
 */
void TxSignCtx_finishInnerHash(TxSignContext* ctx);
/*  @brief Destroy (set with 0) all memory occupied for ctx
 *  @param ctx Context to be destroyed
 */
void TxSignCtx_Destroy(TxSignContext* ctx);
/*  @brief Print inner hash
 *  @param ctx Context with inner hash to print
 */
void TxSignCtx_printInnerHash(TxSignContext* ctx);
/*  @brief Print SHA256 context buffer
 *  @param ctx Context with SHA256 context buffer to be printed
 */
void TxSignCtx_printSHA256(TxSignContext* ctx);
 /* @brief verify_pub_key ec secp256k1
 * @param pub_key pub key to b verified
 * @return true if the verification success
 */
bool verify_pub_key(const uint8_t* pub_key);

#endif
