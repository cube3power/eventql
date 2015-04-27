/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#include "fnord-base/logging.h"
#include "ModelCache.h"

using namespace fnord;

namespace cm {

ModelCache::ModelCache(const String& datadir) : datadir_(datadir) {}

RefCounted* ModelCache::getModel(
    const String& index_name,
    const String& prefix,
    Function<RefCounted* (const String& filename)> load_model) {
  auto filename = getLatestModelFilename(index_name, prefix);

  fnord::logInfo(
      "cm.modelcache",
      "Loading new model $0/$1 from $2",
      index_name,
      prefix,
      filename);
}

String ModelCache::getLatestModelFilename(
    const String& index_name,
    const String& prefix) {
  auto artifacts = getArtifactIndex(index_name);
  auto artifactlist = artifacts->listArtifacts();

  String filename;
  uint64_t built_at = 0;

  for (const auto& a : artifactlist) {
    if (a.files.size() != 1 ||
        !StringUtil::beginsWith(a.name, prefix)) {
      continue;
    }

    String this_built_at;
    for (const auto& a : a.attributes) {
      if (a.first == "built_at") {
        this_built_at = a.second;
        break;
      }
    }

    if (this_built_at.length() == 0) {
      continue;
    }

    auto this_built_at_i = std::stoull(this_built_at);
    if (this_built_at_i > built_at) {
      filename = a.files[0].filename;
      built_at = this_built_at_i;
    }
  }

  if (built_at == 0) {
    RAISEF(kRuntimeError, "no model found for: $0/$1", index_name, prefix);
  }

  return filename;
}

RefPtr<logtable::ArtifactIndex> ModelCache::getArtifactIndex(
    const String& index_name) {
  auto iter = artifacts_.find(index_name);
  if (iter != artifacts_.end()) {
    return iter->second;
  }

  RefPtr<logtable::ArtifactIndex> index(
      new logtable::ArtifactIndex(datadir_, index_name, true));

  artifacts_.emplace(index_name, index);
  return index;
}

} // namespace cm

