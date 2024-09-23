$build_type = $args[0]

if ((Test-Path "C:\Program Files\Microsoft Visual Studio\2022") -or (Test-Path "C:\Program Files (x86)\Microsoft Visual Studio\2022")) {
  $vscode_year = "2022"
} else {
  $vscode_year = "2019"
}

if ([Environment]::Is64BitOperatingSystem) {
  $windows_arch = "win64"
} else {
  $windows_arch = "win32"
}

$dpp_release_information_response = Invoke-WebRequest -Uri "https://dl.dpp.dev/json"
$dpp_release_information = ConvertFrom-Json $dpp_release_information_response.Content
$failed = $true

foreach ($dpp_release in $dpp_release_information) {
  if ($dpp_release.name.contains("$windows_arch-$build_type-vs$vscode_year") -and $dpp_release.url.endswith(".zip")) {
    $failed = $false
    
    Invoke-WebRequest -Uri $dpp_release.url -OutFile dpp.zip
    Expand-Archive -Path .\dpp.zip -DestinationPath .\dpp
    
    $zip_directory = Get-ChildItem -Path .\dpp -Directory -Name
    $inside_include = Get-ChildItem -Path ".\dpp\$zip_directory\include" -Directory -Name
    
    New-Item -Path . -Name deps -ItemType directory -Force
    
    Move-Item -Path ".\dpp\$zip_directory\include\$inside_include\dpp" -Destination .\include\dpp -Force
    Get-ChildItem -Path ".\dpp\$zip_directory\bin" -Recurse -File | Move-Item -Destination .\deps -Force
    Get-ChildItem -Path ".\dpp\$zip_directory\lib\$inside_include" -Recurse -File | Move-Item -Destination .\deps -Force
    
    Remove-Item -Path .\dpp -Recurse -Force
    Remove-Item .\dpp.zip -Force
    
    break
  }
}

if ($failed) {
  $host.ui.WriteErrorLine("Error: unable to find the matching DPP release for this machine.")
}