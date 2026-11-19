$swApp = [System.Runtime.InteropServices.Marshal]::GetActiveObject("SldWorks.Application")
$doc = $swApp.ActiveDoc
if ($doc) {
    Write-Output "Active doc: $($doc.GetTitle())"
}

# Let's inspect carrure.SLDPRT and Base.SLDPRT features
$carrurePath = "c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Solidworks\carrure.SLDPRT"
$basePath = "c:\Users\fbois\Documents\Perso\Projet_Smilewatch\Solidworks\Base.SLDPRT"

$docCarrure = $swApp.OpenDoc6($carrurePath, 1, 1, "", [ref]0, [ref]0)
if ($docCarrure) {
    Write-Output "Carrure opened"
    $feat = $docCarrure.FirstFeature()
    while ($feat) {
        Write-Output "Carrure Feat: $($feat.Name) ($($feat.GetTypeName2()))"
        $feat = $feat.GetNextFeature()
    }
}
