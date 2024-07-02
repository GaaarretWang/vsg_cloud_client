// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_RENDERGEO_RENDERFLATBUFFER_H_
#define FLATBUFFERS_GENERATED_RENDERGEO_RENDERFLATBUFFER_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 5 &&
              FLATBUFFERS_VERSION_REVISION == 26,
             "Non-compatible flatbuffers version included");

namespace RenderFlatBuffer {

struct Instance;
struct InstanceBuilder;

struct Proto;
struct ProtoBuilder;

struct Model;
struct ModelBuilder;

struct Geo;
struct GeoBuilder;

struct Material;
struct MaterialBuilder;

struct RenderFlatBufferDoc;
struct RenderFlatBufferDocBuilder;

struct Instance FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef InstanceBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_INSTANCEID = 4,
    VT_MATERIALID = 6,
    VT_PROTOID = 8,
    VT_PARENTID = 10,
    VT_MATRIX = 12,
    VT_NAME = 14
  };
  const ::flatbuffers::String *InstanceID() const {
    return GetPointer<const ::flatbuffers::String *>(VT_INSTANCEID);
  }
  int32_t MaterialID() const {
    return GetField<int32_t>(VT_MATERIALID, 0);
  }
  const ::flatbuffers::String *ProtoID() const {
    return GetPointer<const ::flatbuffers::String *>(VT_PROTOID);
  }
  const ::flatbuffers::String *ParentID() const {
    return GetPointer<const ::flatbuffers::String *>(VT_PARENTID);
  }
  const ::flatbuffers::Vector<double> *Matrix() const {
    return GetPointer<const ::flatbuffers::Vector<double> *>(VT_MATRIX);
  }
  const ::flatbuffers::String *Name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_INSTANCEID) &&
           verifier.VerifyString(InstanceID()) &&
           VerifyField<int32_t>(verifier, VT_MATERIALID, 4) &&
           VerifyOffset(verifier, VT_PROTOID) &&
           verifier.VerifyString(ProtoID()) &&
           VerifyOffset(verifier, VT_PARENTID) &&
           verifier.VerifyString(ParentID()) &&
           VerifyOffset(verifier, VT_MATRIX) &&
           verifier.VerifyVector(Matrix()) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(Name()) &&
           verifier.EndTable();
  }
};

struct InstanceBuilder {
  typedef Instance Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_InstanceID(::flatbuffers::Offset<::flatbuffers::String> InstanceID) {
    fbb_.AddOffset(Instance::VT_INSTANCEID, InstanceID);
  }
  void add_MaterialID(int32_t MaterialID) {
    fbb_.AddElement<int32_t>(Instance::VT_MATERIALID, MaterialID, 0);
  }
  void add_ProtoID(::flatbuffers::Offset<::flatbuffers::String> ProtoID) {
    fbb_.AddOffset(Instance::VT_PROTOID, ProtoID);
  }
  void add_ParentID(::flatbuffers::Offset<::flatbuffers::String> ParentID) {
    fbb_.AddOffset(Instance::VT_PARENTID, ParentID);
  }
  void add_Matrix(::flatbuffers::Offset<::flatbuffers::Vector<double>> Matrix) {
    fbb_.AddOffset(Instance::VT_MATRIX, Matrix);
  }
  void add_Name(::flatbuffers::Offset<::flatbuffers::String> Name) {
    fbb_.AddOffset(Instance::VT_NAME, Name);
  }
  explicit InstanceBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Instance> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Instance>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Instance> CreateInstance(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> InstanceID = 0,
    int32_t MaterialID = 0,
    ::flatbuffers::Offset<::flatbuffers::String> ProtoID = 0,
    ::flatbuffers::Offset<::flatbuffers::String> ParentID = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<double>> Matrix = 0,
    ::flatbuffers::Offset<::flatbuffers::String> Name = 0) {
  InstanceBuilder builder_(_fbb);
  builder_.add_Name(Name);
  builder_.add_Matrix(Matrix);
  builder_.add_ParentID(ParentID);
  builder_.add_ProtoID(ProtoID);
  builder_.add_MaterialID(MaterialID);
  builder_.add_InstanceID(InstanceID);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Instance> CreateInstanceDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *InstanceID = nullptr,
    int32_t MaterialID = 0,
    const char *ProtoID = nullptr,
    const char *ParentID = nullptr,
    const std::vector<double> *Matrix = nullptr,
    const char *Name = nullptr) {
  auto InstanceID__ = InstanceID ? _fbb.CreateString(InstanceID) : 0;
  auto ProtoID__ = ProtoID ? _fbb.CreateString(ProtoID) : 0;
  auto ParentID__ = ParentID ? _fbb.CreateString(ParentID) : 0;
  auto Matrix__ = Matrix ? _fbb.CreateVector<double>(*Matrix) : 0;
  auto Name__ = Name ? _fbb.CreateString(Name) : 0;
  return RenderFlatBuffer::CreateInstance(
      _fbb,
      InstanceID__,
      MaterialID,
      ProtoID__,
      ParentID__,
      Matrix__,
      Name__);
}

struct Proto FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef ProtoBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_PROTOID = 4,
    VT_NAME = 6,
    VT_TYPE = 8,
    VT_MODELS = 10
  };
  const ::flatbuffers::String *ProtoID() const {
    return GetPointer<const ::flatbuffers::String *>(VT_PROTOID);
  }
  const ::flatbuffers::String *Name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *Type() const {
    return GetPointer<const ::flatbuffers::String *>(VT_TYPE);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Model>> *Models() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Model>> *>(VT_MODELS);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_PROTOID) &&
           verifier.VerifyString(ProtoID()) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(Name()) &&
           VerifyOffset(verifier, VT_TYPE) &&
           verifier.VerifyString(Type()) &&
           VerifyOffset(verifier, VT_MODELS) &&
           verifier.VerifyVector(Models()) &&
           verifier.VerifyVectorOfTables(Models()) &&
           verifier.EndTable();
  }
};

struct ProtoBuilder {
  typedef Proto Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_ProtoID(::flatbuffers::Offset<::flatbuffers::String> ProtoID) {
    fbb_.AddOffset(Proto::VT_PROTOID, ProtoID);
  }
  void add_Name(::flatbuffers::Offset<::flatbuffers::String> Name) {
    fbb_.AddOffset(Proto::VT_NAME, Name);
  }
  void add_Type(::flatbuffers::Offset<::flatbuffers::String> Type) {
    fbb_.AddOffset(Proto::VT_TYPE, Type);
  }
  void add_Models(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Model>>> Models) {
    fbb_.AddOffset(Proto::VT_MODELS, Models);
  }
  explicit ProtoBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Proto> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Proto>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Proto> CreateProto(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> ProtoID = 0,
    ::flatbuffers::Offset<::flatbuffers::String> Name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> Type = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Model>>> Models = 0) {
  ProtoBuilder builder_(_fbb);
  builder_.add_Models(Models);
  builder_.add_Type(Type);
  builder_.add_Name(Name);
  builder_.add_ProtoID(ProtoID);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Proto> CreateProtoDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *ProtoID = nullptr,
    const char *Name = nullptr,
    const char *Type = nullptr,
    const std::vector<::flatbuffers::Offset<RenderFlatBuffer::Model>> *Models = nullptr) {
  auto ProtoID__ = ProtoID ? _fbb.CreateString(ProtoID) : 0;
  auto Name__ = Name ? _fbb.CreateString(Name) : 0;
  auto Type__ = Type ? _fbb.CreateString(Type) : 0;
  auto Models__ = Models ? _fbb.CreateVector<::flatbuffers::Offset<RenderFlatBuffer::Model>>(*Models) : 0;
  return RenderFlatBuffer::CreateProto(
      _fbb,
      ProtoID__,
      Name__,
      Type__,
      Models__);
}

struct Model FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef ModelBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TYPE = 4,
    VT_MATERIALID = 6,
    VT_GEO = 8
  };
  const ::flatbuffers::String *Type() const {
    return GetPointer<const ::flatbuffers::String *>(VT_TYPE);
  }
  int32_t MaterialID() const {
    return GetField<int32_t>(VT_MATERIALID, 0);
  }
  const RenderFlatBuffer::Geo *Geo() const {
    return GetPointer<const RenderFlatBuffer::Geo *>(VT_GEO);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_TYPE) &&
           verifier.VerifyString(Type()) &&
           VerifyField<int32_t>(verifier, VT_MATERIALID, 4) &&
           VerifyOffset(verifier, VT_GEO) &&
           verifier.VerifyTable(Geo()) &&
           verifier.EndTable();
  }
};

struct ModelBuilder {
  typedef Model Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_Type(::flatbuffers::Offset<::flatbuffers::String> Type) {
    fbb_.AddOffset(Model::VT_TYPE, Type);
  }
  void add_MaterialID(int32_t MaterialID) {
    fbb_.AddElement<int32_t>(Model::VT_MATERIALID, MaterialID, 0);
  }
  void add_Geo(::flatbuffers::Offset<RenderFlatBuffer::Geo> Geo) {
    fbb_.AddOffset(Model::VT_GEO, Geo);
  }
  explicit ModelBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Model> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Model>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Model> CreateModel(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> Type = 0,
    int32_t MaterialID = 0,
    ::flatbuffers::Offset<RenderFlatBuffer::Geo> Geo = 0) {
  ModelBuilder builder_(_fbb);
  builder_.add_Geo(Geo);
  builder_.add_MaterialID(MaterialID);
  builder_.add_Type(Type);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Model> CreateModelDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *Type = nullptr,
    int32_t MaterialID = 0,
    ::flatbuffers::Offset<RenderFlatBuffer::Geo> Geo = 0) {
  auto Type__ = Type ? _fbb.CreateString(Type) : 0;
  return RenderFlatBuffer::CreateModel(
      _fbb,
      Type__,
      MaterialID,
      Geo);
}

struct Geo FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef GeoBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_POSITION = 4,
    VT_NORMAL = 6,
    VT_UV = 8,
    VT_INDEX = 10
  };
  const ::flatbuffers::Vector<float> *Position() const {
    return GetPointer<const ::flatbuffers::Vector<float> *>(VT_POSITION);
  }
  const ::flatbuffers::Vector<float> *Normal() const {
    return GetPointer<const ::flatbuffers::Vector<float> *>(VT_NORMAL);
  }
  const ::flatbuffers::Vector<float> *UV() const {
    return GetPointer<const ::flatbuffers::Vector<float> *>(VT_UV);
  }
  const ::flatbuffers::Vector<int32_t> *Index() const {
    return GetPointer<const ::flatbuffers::Vector<int32_t> *>(VT_INDEX);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_POSITION) &&
           verifier.VerifyVector(Position()) &&
           VerifyOffset(verifier, VT_NORMAL) &&
           verifier.VerifyVector(Normal()) &&
           VerifyOffset(verifier, VT_UV) &&
           verifier.VerifyVector(UV()) &&
           VerifyOffset(verifier, VT_INDEX) &&
           verifier.VerifyVector(Index()) &&
           verifier.EndTable();
  }
};

struct GeoBuilder {
  typedef Geo Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_Position(::flatbuffers::Offset<::flatbuffers::Vector<float>> Position) {
    fbb_.AddOffset(Geo::VT_POSITION, Position);
  }
  void add_Normal(::flatbuffers::Offset<::flatbuffers::Vector<float>> Normal) {
    fbb_.AddOffset(Geo::VT_NORMAL, Normal);
  }
  void add_UV(::flatbuffers::Offset<::flatbuffers::Vector<float>> UV) {
    fbb_.AddOffset(Geo::VT_UV, UV);
  }
  void add_Index(::flatbuffers::Offset<::flatbuffers::Vector<int32_t>> Index) {
    fbb_.AddOffset(Geo::VT_INDEX, Index);
  }
  explicit GeoBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Geo> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Geo>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Geo> CreateGeo(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<float>> Position = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<float>> Normal = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<float>> UV = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<int32_t>> Index = 0) {
  GeoBuilder builder_(_fbb);
  builder_.add_Index(Index);
  builder_.add_UV(UV);
  builder_.add_Normal(Normal);
  builder_.add_Position(Position);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Geo> CreateGeoDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<float> *Position = nullptr,
    const std::vector<float> *Normal = nullptr,
    const std::vector<float> *UV = nullptr,
    const std::vector<int32_t> *Index = nullptr) {
  auto Position__ = Position ? _fbb.CreateVector<float>(*Position) : 0;
  auto Normal__ = Normal ? _fbb.CreateVector<float>(*Normal) : 0;
  auto UV__ = UV ? _fbb.CreateVector<float>(*UV) : 0;
  auto Index__ = Index ? _fbb.CreateVector<int32_t>(*Index) : 0;
  return RenderFlatBuffer::CreateGeo(
      _fbb,
      Position__,
      Normal__,
      UV__,
      Index__);
}

struct Material FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef MaterialBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_MATERIALID = 4,
    VT_DIFFUSE = 6,
    VT_SPECULAR = 8,
    VT_SHININESS = 10,
    VT_DIFFUSETEXTURENAME = 12,
    VT_EMISSIVE = 14,
    VT_EMISSIVEINTENSITY = 16,
    VT_METALNESS = 18,
    VT_ROUGHNESS = 20,
    VT_TRANSMISSION = 22
  };
  int32_t MaterialID() const {
    return GetField<int32_t>(VT_MATERIALID, 0);
  }
  const ::flatbuffers::String *Diffuse() const {
    return GetPointer<const ::flatbuffers::String *>(VT_DIFFUSE);
  }
  const ::flatbuffers::String *Specular() const {
    return GetPointer<const ::flatbuffers::String *>(VT_SPECULAR);
  }
  float Shininess() const {
    return GetField<float>(VT_SHININESS, 0.0f);
  }
  const ::flatbuffers::String *DiffuseTextureName() const {
    return GetPointer<const ::flatbuffers::String *>(VT_DIFFUSETEXTURENAME);
  }
  const ::flatbuffers::String *Emissive() const {
    return GetPointer<const ::flatbuffers::String *>(VT_EMISSIVE);
  }
  float EmissiveIntensity() const {
    return GetField<float>(VT_EMISSIVEINTENSITY, 0.0f);
  }
  float Metalness() const {
    return GetField<float>(VT_METALNESS, 0.0f);
  }
  float Roughness() const {
    return GetField<float>(VT_ROUGHNESS, 0.0f);
  }
  float Transmission() const {
    return GetField<float>(VT_TRANSMISSION, 0.0f);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_MATERIALID, 4) &&
           VerifyOffset(verifier, VT_DIFFUSE) &&
           verifier.VerifyString(Diffuse()) &&
           VerifyOffset(verifier, VT_SPECULAR) &&
           verifier.VerifyString(Specular()) &&
           VerifyField<float>(verifier, VT_SHININESS, 4) &&
           VerifyOffset(verifier, VT_DIFFUSETEXTURENAME) &&
           verifier.VerifyString(DiffuseTextureName()) &&
           VerifyOffset(verifier, VT_EMISSIVE) &&
           verifier.VerifyString(Emissive()) &&
           VerifyField<float>(verifier, VT_EMISSIVEINTENSITY, 4) &&
           VerifyField<float>(verifier, VT_METALNESS, 4) &&
           VerifyField<float>(verifier, VT_ROUGHNESS, 4) &&
           VerifyField<float>(verifier, VT_TRANSMISSION, 4) &&
           verifier.EndTable();
  }
};

struct MaterialBuilder {
  typedef Material Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_MaterialID(int32_t MaterialID) {
    fbb_.AddElement<int32_t>(Material::VT_MATERIALID, MaterialID, 0);
  }
  void add_Diffuse(::flatbuffers::Offset<::flatbuffers::String> Diffuse) {
    fbb_.AddOffset(Material::VT_DIFFUSE, Diffuse);
  }
  void add_Specular(::flatbuffers::Offset<::flatbuffers::String> Specular) {
    fbb_.AddOffset(Material::VT_SPECULAR, Specular);
  }
  void add_Shininess(float Shininess) {
    fbb_.AddElement<float>(Material::VT_SHININESS, Shininess, 0.0f);
  }
  void add_DiffuseTextureName(::flatbuffers::Offset<::flatbuffers::String> DiffuseTextureName) {
    fbb_.AddOffset(Material::VT_DIFFUSETEXTURENAME, DiffuseTextureName);
  }
  void add_Emissive(::flatbuffers::Offset<::flatbuffers::String> Emissive) {
    fbb_.AddOffset(Material::VT_EMISSIVE, Emissive);
  }
  void add_EmissiveIntensity(float EmissiveIntensity) {
    fbb_.AddElement<float>(Material::VT_EMISSIVEINTENSITY, EmissiveIntensity, 0.0f);
  }
  void add_Metalness(float Metalness) {
    fbb_.AddElement<float>(Material::VT_METALNESS, Metalness, 0.0f);
  }
  void add_Roughness(float Roughness) {
    fbb_.AddElement<float>(Material::VT_ROUGHNESS, Roughness, 0.0f);
  }
  void add_Transmission(float Transmission) {
    fbb_.AddElement<float>(Material::VT_TRANSMISSION, Transmission, 0.0f);
  }
  explicit MaterialBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Material> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Material>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Material> CreateMaterial(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t MaterialID = 0,
    ::flatbuffers::Offset<::flatbuffers::String> Diffuse = 0,
    ::flatbuffers::Offset<::flatbuffers::String> Specular = 0,
    float Shininess = 0.0f,
    ::flatbuffers::Offset<::flatbuffers::String> DiffuseTextureName = 0,
    ::flatbuffers::Offset<::flatbuffers::String> Emissive = 0,
    float EmissiveIntensity = 0.0f,
    float Metalness = 0.0f,
    float Roughness = 0.0f,
    float Transmission = 0.0f) {
  MaterialBuilder builder_(_fbb);
  builder_.add_Transmission(Transmission);
  builder_.add_Roughness(Roughness);
  builder_.add_Metalness(Metalness);
  builder_.add_EmissiveIntensity(EmissiveIntensity);
  builder_.add_Emissive(Emissive);
  builder_.add_DiffuseTextureName(DiffuseTextureName);
  builder_.add_Shininess(Shininess);
  builder_.add_Specular(Specular);
  builder_.add_Diffuse(Diffuse);
  builder_.add_MaterialID(MaterialID);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Material> CreateMaterialDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int32_t MaterialID = 0,
    const char *Diffuse = nullptr,
    const char *Specular = nullptr,
    float Shininess = 0.0f,
    const char *DiffuseTextureName = nullptr,
    const char *Emissive = nullptr,
    float EmissiveIntensity = 0.0f,
    float Metalness = 0.0f,
    float Roughness = 0.0f,
    float Transmission = 0.0f) {
  auto Diffuse__ = Diffuse ? _fbb.CreateString(Diffuse) : 0;
  auto Specular__ = Specular ? _fbb.CreateString(Specular) : 0;
  auto DiffuseTextureName__ = DiffuseTextureName ? _fbb.CreateString(DiffuseTextureName) : 0;
  auto Emissive__ = Emissive ? _fbb.CreateString(Emissive) : 0;
  return RenderFlatBuffer::CreateMaterial(
      _fbb,
      MaterialID,
      Diffuse__,
      Specular__,
      Shininess,
      DiffuseTextureName__,
      Emissive__,
      EmissiveIntensity,
      Metalness,
      Roughness,
      Transmission);
}

struct RenderFlatBufferDoc FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef RenderFlatBufferDocBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_BOM = 4,
    VT_MATERIALDATA = 6,
    VT_PROTODATA = 8
  };
  const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Instance>> *Bom() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Instance>> *>(VT_BOM);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Material>> *MaterialData() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Material>> *>(VT_MATERIALDATA);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Proto>> *ProtoData() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Proto>> *>(VT_PROTODATA);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_BOM) &&
           verifier.VerifyVector(Bom()) &&
           verifier.VerifyVectorOfTables(Bom()) &&
           VerifyOffset(verifier, VT_MATERIALDATA) &&
           verifier.VerifyVector(MaterialData()) &&
           verifier.VerifyVectorOfTables(MaterialData()) &&
           VerifyOffset(verifier, VT_PROTODATA) &&
           verifier.VerifyVector(ProtoData()) &&
           verifier.VerifyVectorOfTables(ProtoData()) &&
           verifier.EndTable();
  }
};

struct RenderFlatBufferDocBuilder {
  typedef RenderFlatBufferDoc Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_Bom(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Instance>>> Bom) {
    fbb_.AddOffset(RenderFlatBufferDoc::VT_BOM, Bom);
  }
  void add_MaterialData(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Material>>> MaterialData) {
    fbb_.AddOffset(RenderFlatBufferDoc::VT_MATERIALDATA, MaterialData);
  }
  void add_ProtoData(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Proto>>> ProtoData) {
    fbb_.AddOffset(RenderFlatBufferDoc::VT_PROTODATA, ProtoData);
  }
  explicit RenderFlatBufferDocBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<RenderFlatBufferDoc> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<RenderFlatBufferDoc>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<RenderFlatBufferDoc> CreateRenderFlatBufferDoc(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Instance>>> Bom = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Material>>> MaterialData = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<RenderFlatBuffer::Proto>>> ProtoData = 0) {
  RenderFlatBufferDocBuilder builder_(_fbb);
  builder_.add_ProtoData(ProtoData);
  builder_.add_MaterialData(MaterialData);
  builder_.add_Bom(Bom);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<RenderFlatBufferDoc> CreateRenderFlatBufferDocDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<::flatbuffers::Offset<RenderFlatBuffer::Instance>> *Bom = nullptr,
    const std::vector<::flatbuffers::Offset<RenderFlatBuffer::Material>> *MaterialData = nullptr,
    const std::vector<::flatbuffers::Offset<RenderFlatBuffer::Proto>> *ProtoData = nullptr) {
  auto Bom__ = Bom ? _fbb.CreateVector<::flatbuffers::Offset<RenderFlatBuffer::Instance>>(*Bom) : 0;
  auto MaterialData__ = MaterialData ? _fbb.CreateVector<::flatbuffers::Offset<RenderFlatBuffer::Material>>(*MaterialData) : 0;
  auto ProtoData__ = ProtoData ? _fbb.CreateVector<::flatbuffers::Offset<RenderFlatBuffer::Proto>>(*ProtoData) : 0;
  return RenderFlatBuffer::CreateRenderFlatBufferDoc(
      _fbb,
      Bom__,
      MaterialData__,
      ProtoData__);
}

inline const RenderFlatBuffer::RenderFlatBufferDoc *GetRenderFlatBufferDoc(const void *buf) {
  return ::flatbuffers::GetRoot<RenderFlatBuffer::RenderFlatBufferDoc>(buf);
}

inline const RenderFlatBuffer::RenderFlatBufferDoc *GetSizePrefixedRenderFlatBufferDoc(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<RenderFlatBuffer::RenderFlatBufferDoc>(buf);
}

inline bool VerifyRenderFlatBufferDocBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<RenderFlatBuffer::RenderFlatBufferDoc>(nullptr);
}

inline bool VerifySizePrefixedRenderFlatBufferDocBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<RenderFlatBuffer::RenderFlatBufferDoc>(nullptr);
}

inline void FinishRenderFlatBufferDocBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<RenderFlatBuffer::RenderFlatBufferDoc> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedRenderFlatBufferDocBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<RenderFlatBuffer::RenderFlatBufferDoc> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace RenderFlatBuffer

#endif  // FLATBUFFERS_GENERATED_RENDERGEO_RENDERFLATBUFFER_H_
