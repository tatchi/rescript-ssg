[@send] external replaceAll: (string, string, string) => string = "replaceAll";

// 'v16.15.0' => 16150
let nodeVersionToInt = (s: string) => {
  let refinedNodeVersion = s->replaceAll("v", "")->replaceAll(".", "");
  Belt.Int.fromString(refinedNodeVersion)->Belt.Option.getWithDefault(0);
};

let bsArtifactRegex = [%re {|/file:.*\.bs\.js$/i|}];

let isBsArtifact = fileUrl => {
  Js.String2.match(fileUrl, bsArtifactRegex) != None;
};

let assetRegex = [%re
  {|/\.(css|jpg|jpeg|png|gif|svg|ico|avif|webp|woff|woff2|json|mp4)$/i|}
];

let isAsset = fileUrl => {
  Js.String2.match(fileUrl, assetRegex) != None;
};

let webpackAssetsDir = "assets";

// We get a file's hash and make a JS module that exports a filename with hash suffix.
let getFinalHashedAssetPath =
    (url: string, processFileData: option(Buffer.t => Buffer.t)) => {
  let filePath = url->Js.String2.replace("file://", "");

  let fileData = Fs.readFileSyncAsBuffer(filePath);

  let processedFileData =
    switch (processFileData) {
    | None => fileData
    | Some(func) => func(fileData)
    };

  let fileHash = Crypto.Hash.bufferToHash(processedFileData);

  let fileName = Path.basename(url);

  let fileExt = Path.extname(fileName);

  let filenameWithoutExt = fileName->Js.String2.replace(fileExt, "");

  let filenameWithHash = {j|$(filenameWithoutExt).$(fileHash)$(fileExt)|j};

  let assetPath =
    switch (CliArgs.assetPrefix->Js.String2.startsWith("https://")) {
    | false =>
      let assetsDir = Path.join2(CliArgs.assetPrefix, webpackAssetsDir);
      Path.join2(assetsDir, filenameWithHash);
    | true =>
      let assetsDir = CliArgs.assetPrefix ++ "/" ++ webpackAssetsDir;
      assetsDir ++ "/" ++ filenameWithHash;
    };

  let assetPath = Utils.maybeAddSlashPrefix(assetPath);

  assetPath;
};

let processAsset =
    (url: string, processFileData: option(Buffer.t => Buffer.t)) => {
  let webpackAssetPath = getFinalHashedAssetPath(url, processFileData);

  Js.Promise.resolve({
    "format": "module",
    "source": {j|export default "$(webpackAssetPath)"|j},
    // shortCircuit is needed since node v16.17.0
    "shortCircuit": true,
  });
};
