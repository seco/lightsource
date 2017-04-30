var config;


$(document).ready(function () {
    config = $("#lightsource-config-table");
    configStripAdd = $(".lightsource-add-strip");
    configTest      = $(".lightsource-test-config");
    tmplStrip            = $.templates("#lightsource-strip-template");
    $.jsonRPC.request("init", {success: remoteConfigReceived, error: remoteConfigReceptionError});
    $(configStripAdd).click(function (e) {
        $(config).append(createStripRow(null,0));
    });

    $(configTest).click(function (e){
        console.log($('#lightsource-config-form').serializeObject().strips);
        $.jsonRPC.request("testConfig",
            {
                params:[$('form#lightsource-config-form').serializeObject().strips],
                success: testConfigReceived,
                error: testConfigReceptionError
            }
        );
    });

});

$.jsonRPC.setup({
    endPoint: "/devel/lightsource/data/rpc.php",
    namespace: "lightsource"
});

function removeSlice(slice)
{
    slice.remove();
}
function removeStrip(strip)
{
    strip.remove();
}

function renderSlicesTable(strip, slices)
{

}

function renderStripsTable(strips)
{
    console.log(strips);
    $("#lightsource-config-form").html(tmplStrip.render({strips: strips}));
}

function remoteConfigReceived(remoteConfig)
{
    console.log("Received remote configuration");
    console.log(remoteConfig.result);

    if (remoteConfig.result.strips.length > 0)
    {
        renderStripsTable(remoteConfig.result.strips);
    }
    
    $(function()
    {
        $(".colorpicker-selector").colorpicker({format: 'hex'}).on('changeColor',function(e)
        {
            $(e.target.nextElementSibling).css("background-color", e.value);
        }); }
    );
}

function remoteConfigReceptionError(o)
{
    console.log("Error receiving initial config from remote server");
}

function testConfigReceived(o)
{
    console.log("Config test success");
}

function testConfigReceptionError(o)
{
    console.log("Config test failed");
}