$swApp = [System.Runtime.InteropServices.Marshal]::GetActiveObject("SldWorks.Application")
if (!$swApp) {
    Write-Output "SW not running in ROT"
    exit
}

$asmDoc = $swApp.ActiveDoc
if ($asmDoc) {
    $comps = $asmDoc.GetComponents($false)
    foreach ($c in $comps) {
        $name = $c.Name2
        if ($name -like "*Batterie*" -or $name -like "*Carte_V2*" -or $name -like "*carrure*") {
            $xform = $c.Transform2
            $data = $xform.ArrayData
            # Translation is at index 9, 10, 11 (in meters)
            $tx = [math]::Round($data[9] * 1000, 3)
            $ty = [math]::Round($data[10] * 1000, 3)
            $tz = [math]::Round($data[11] * 1000, 3)
            Write-Output "Component $name : X=$tx mm, Y=$ty mm, Z=$tz mm"
        }
    }
}
