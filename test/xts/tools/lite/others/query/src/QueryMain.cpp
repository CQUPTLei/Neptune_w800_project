/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "parameter.h"
#include <stdio.h>
#include <stdlib.h>

void ObtainProductParms()
{
    char *bootloaderVersion = GetBootloaderVersion();
    if (bootloaderVersion != nullptr) {
        printf("The bootloaderVersion is [%s]\n", bootloaderVersion);
        free(bootloaderVersion);
    }

    char *securityPatchTag = GetSecurityPatchTag();
    if (securityPatchTag != nullptr) {
        printf("The Security Patch is [%s]\n", securityPatchTag);
        free(securityPatchTag);
    }

    char *abiList = GetAbiList();
    if (abiList != nullptr) {
        printf("The AbiList is [%s]\n", abiList);
        free(abiList);
    }

    char *sdkApiLevel = GetSdkApiLevel();
    if (sdkApiLevel != nullptr) {
        printf("The sdkApiLevel is [%s]\n", sdkApiLevel);
        free(sdkApiLevel);
    }

    char *firstApiLevel = GetFirstApiLevel();
    if (firstApiLevel != nullptr) {
        printf("The firstApiLevel is [%s]\n", firstApiLevel);
        free(firstApiLevel);
    }

    char *incrementalVersion = GetIncrementalVersion();
    if (incrementalVersion != nullptr) {
        printf("The productSeries is [%s]\n", incrementalVersion);
        free(incrementalVersion);
    }

    char *versionId = GetVersionId();
    if (versionId != nullptr) {
        printf("The VersionID is [%s]\n", versionId);
        free(versionId);
    }

    char *buildType = GetBuildType();
    if (buildType != nullptr) {
        printf("The buildType is [%s]\n", buildType);
        free(buildType);
    }

    char *buildUser = GetBuildUser();
    if (buildUser != nullptr) {
        printf("The buildUser is [%s]\n", buildUser);
        free(buildUser);
    }

    char *buildHost = GetBuildHost();
    if (buildHost != nullptr) {
        printf("The buildHost is [%s]\n", buildHost);
        free(buildHost);
    }

    char *buildTime = GetBuildTime();
    if (buildTime != nullptr) {
        printf("The buildTime is [%s]\n", buildTime);
        free(buildTime);
    }

    char *buildRootHash = GetBuildRootHash();
    if (buildRootHash != nullptr) {
        printf("The BuildRootHash is [%s]\n", buildRootHash);
        free(buildRootHash);
    }	
}

int main()
{
    printf("******To Obtain Product Params Start******\n");
    char *productType = GetProductType();
    if (productType != nullptr) {
        printf("The Product Type is [%s]\n", productType);
        free(productType);
    }

    char *manuFacture = GetManufacture();
    if (manuFacture != nullptr) {
        printf("The manuFacture is [%s]\n", manuFacture);
        free(manuFacture);
    }

    char *brand = GetBrand();
    if (brand != nullptr) {
        printf("The brand is [%s]\n", brand);
        free(brand);
    }

    char *marketName = GetMarketName();
    if (marketName != nullptr) {
        printf("The marketName is [%s]\n", marketName);
        free(marketName);
    }

    char *productSeries = GetProductSeries();
    if (productSeries != nullptr) {
        printf("The productSeries is [%s]\n", productSeries);
        free(productSeries);
    }

    char *softwareModel = GetSoftwareModel();
    if (softwareModel != nullptr) {
        printf("The softwareModel is [%s]\n", softwareModel);
        free(softwareModel);
    }

    char *hardWareModel = GetHardwareModel();
    if (hardWareModel != nullptr) {
        printf("The HardwareModel is [%s]\n", hardWareModel);
        free(hardWareModel);
    }

    char *hardWareProfile = GetHardwareProfile();
    if (hardWareProfile != nullptr) {
        printf("The HardwareProfile is [%s]\n", hardWareProfile);
        free(hardWareProfile);
    }

    char *serial = GetSerial();
    if (serial != nullptr) {
        printf("The serial is [%s]\n", serial);
        free(serial);
    }

    char *osName = GetOsName();
    if (osName != nullptr) {
        printf("The osName is [%s]\n", osName);
        free(osName);
    }

    char *displayVersion = GetDisplayVersion();
    if (displayVersion != nullptr) {
        printf("The OS Version is [%s]\n", displayVersion);
        free(displayVersion);
    }

    ObtainProductParms();	

    printf("******To Obtain Product Params End  ******\n");
    return 0;
}
